#include "SaveManager.h"

#include <asw/asw.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Character.h"
#include "World.h"
#include "manager/InterfaceTypeManager.h"

namespace {
constexpr int SAVE_VERSION = 1;

// Windows whose contents are saved
constexpr std::array<const char*, 3> SAVED_INVENTORIES = {
    "inventory", "crafting", "furnace"};

nlohmann::json inventoryToJson(Inventory& inventory) {
  auto stacks = nlohmann::json::array();

  for (int i = 0; i < inventory.getSize(); i++) {
    const auto stack = inventory.getStack(i);
    if (stack && stack->getItem()) {
      stacks.push_back({stack->getItem()->getType().getId(),
                        stack->getItem()->getMeta(), stack->getQuantity()});
    } else {
      stacks.push_back(nullptr);
    }
  }

  return stacks;
}

void inventoryFromJson(Inventory& inventory, const nlohmann::json& stacks) {
  inventory.empty();

  for (int i = 0; i < inventory.getSize() && i < static_cast<int>(stacks.size());
       i++) {
    const auto& stack = stacks[i];
    if (stack.is_null()) {
      continue;
    }

    inventory.getStack(i)->setItem(
        std::make_shared<Item>(stack[0].get<std::string>(),
                               stack[1].get<int>()),
        stack[2].get<int>());
  }
}
}  // namespace

std::string SaveManager::path() {
#ifdef __EMSCRIPTEN__
  // Mounted as IDBFS in main
  return "/save/save.json";
#else
  static const std::string cached =
      asw::assets::get_save_path("AdsGames", "JimFarm") + "save.json";
  return cached;
#endif
}

bool SaveManager::exists() {
  std::error_code error;
  return std::filesystem::exists(path(), error);
}

bool SaveManager::save(const World& world, const Character& player) {
  nlohmann::json data = world.toJson();
  data["version"] = SAVE_VERSION;
  data["player"] = {player.getPosition().x, player.getPosition().y};

  for (const auto* name : SAVED_INVENTORIES) {
    data["inventories"][name] = inventoryToJson(
        *InterfaceTypeManager::getInterfaceByName(name).getInventory());
  }

  // Write to a temp file first so a crash can not corrupt the save
  const auto final_path = path();
  const auto temp_path = final_path + ".tmp";

  {
    std::ofstream file(temp_path);
    if (!file.is_open()) {
      asw::log::warn("Could not write save {}", temp_path);
      return false;
    }
    file << data.dump();
  }

  std::error_code error;
  std::filesystem::rename(temp_path, final_path, error);
  if (error) {
    asw::log::warn("Could not replace save {}", final_path);
    return false;
  }

#ifdef __EMSCRIPTEN__
  // Flush to IndexedDB
  EM_ASM(FS.syncfs(false, function(err){}););
#endif

  asw::log::info("Saved game to {}", final_path);
  return true;
}

bool SaveManager::load(World& world, Character& player) {
  std::ifstream file(path());
  if (!file.is_open()) {
    return false;
  }

  try {
    const auto data = nlohmann::json::parse(file);
    if (data.value("version", 0) != SAVE_VERSION) {
      asw::log::warn("Save version not supported");
      return false;
    }

    world.fromJson(data);

    for (const auto* name : SAVED_INVENTORIES) {
      if (data["inventories"].contains(name)) {
        inventoryFromJson(
            *InterfaceTypeManager::getInterfaceByName(name).getInventory(),
            data["inventories"][name]);
      }
    }

    player.setPosition(asw::Vec2i(data["player"][0], data["player"][1]));
  } catch (const std::exception& e) {
    asw::log::warn("Could not load save: {}", e.what());
    return false;
  }

  asw::log::info("Loaded game from {}", path());
  return true;
}
