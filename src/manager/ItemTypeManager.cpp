#include "ItemTypeManager.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Chunk.h"
#include "../Tile.h"
#include "../utility/Tools.h"
#include "TileTypeManager.h"

std::map<std::string, TileType> ItemTypeManager::item_defs;
std::map<std::string, ItemInfo> ItemTypeManager::item_info;

int layerFromName(const std::string& name) {
  if (name == "background") {
    return LAYER_BACKGROUND;
  }
  if (name == "midground") {
    return LAYER_MIDGROUND;
  }
  if (name == "foreground") {
    return LAYER_FOREGROUND;
  }

  throw std::runtime_error("Unknown layer: " + name);
}

namespace {
std::vector<int> parseLayers(const nlohmann::json& data, const char* key) {
  std::vector<int> layers;
  if (data.contains(key)) {
    for (const std::string name : data[key]) {
      layers.push_back(layerFromName(name));
    }
  }
  return layers;
}

ItemAction parseAction(const nlohmann::json& data) {
  ItemAction action;

  if (data.contains("layer")) {
    action.layer = layerFromName(data["layer"]);
  }

  action.tiles = data.value("tiles", std::vector<std::string>{});
  action.empty = data.value("empty", false);
  action.requires_empty = parseLayers(data, "requires_empty");
  action.requires_present = parseLayers(data, "requires_present");
  action.min_meta = data.value("min_meta", -1);
  action.max_meta = data.value("max_meta", -1);

  // "replace": null removes the tile
  if (data.contains("replace")) {
    action.has_replace = true;
    action.replace =
        data["replace"].is_null() ? "" : data["replace"].get<std::string>();
    action.replace_meta = data.value("replace_meta", 0);
  }

  if (data.contains("place")) {
    action.place = data["place"]["tile"];
    action.place_layer = layerFromName(data["place"]["layer"]);
    action.place_meta = data["place"].value("meta", 0);
  }

  action.drops = data.value("drops", std::vector<std::string>{});
  action.gives = data.value("give", std::vector<std::string>{});
  action.consume = data.value("consume", false);
  action.set_meta = data.value("set_meta", -1);
  action.add_meta = data.value("add_meta", 0);
  action.sound = data.value("sound", "");
  action.message = data.value("message", "");
  action.particles = data.value("particles", "");
  action.energy = data.value("energy", 0.0F);
  action.area = data.value("area", 0);
  action.hunger = data.value("hunger", 0.0F);
  action.thirst = data.value("thirst", 0.0F);
  action.health = data.value("health", 0.0F);
  action.fail = data.value("fail", false);
  action.verb = data.value("verb", "");

  // Guess a word for the cursor when none is given
  if (action.verb.empty() && !action.fail) {
    if (!action.place.empty()) {
      action.verb = "Place";
    } else if (!action.gives.empty()) {
      action.verb = "Cook";
    } else if (action.set_meta >= 0) {
      action.verb = "Fill";
    } else if (action.thirst > 0.0F) {
      action.verb = "Drink";
    } else if (action.has_replace && action.replace.empty()) {
      action.verb = "Clear";
    } else {
      action.verb = "Use";
    }
  }

  return action;
}
}  // namespace

// Load tiles
int ItemTypeManager::loadItems(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Parse data
  for (auto const& item : nlohmann::json::parse(file)) {
    // Read from json
    std::string name = item["name"];
    unsigned char image_x = item["image_x"];
    unsigned char image_y = item["image_y"];
    std::string id = item["id"];

    // Create item, set variables and add it to the item list
    item_defs[id] = TileType(1, 1, id, name, 0);
    item_defs[id].setSpriteSheet(
        TileTypeManager::getSheet(item.value("sheet", "items")));
    item_defs[id].setImageType("", 1, 1, image_x, image_y, 1, 1);

    if (item.contains("tint")) {
      item_defs[id].setTint(
          asw::Color(item["tint"][0], item["tint"][1], item["tint"][2]));
    }

    // Gameplay data
    ItemInfo info;
    info.description = item.value("description", "");
    info.price = item.value("price", 0);
    info.value = item.value("value", 0);
    info.attack = item.value("attack", 0);
    info.power = item.value("power", 0.0F);
    info.reach = item.value("reach", 1.5F);
    info.arc = item.value("arc", 60.0F);
    info.cooldown = item.value("cooldown", 0.5F);
    info.throwable = item.value("throw", false);
    info.carry_warmth = item.value("carry_warmth", 0.0F);
    info.start_meta = item.value("start_meta", 0);

    if (item.contains("food")) {
      const auto& food = item["food"];
      info.edible = true;
      info.hunger = food.value("hunger", 0.0F);
      info.thirst = food.value("thirst", 0.0F);
      info.health = food.value("health", 0.0F);
      info.warmth = food.value("warmth", 0.0F);
      info.food_uses_meta = food.value("uses_meta", false);
    }

    if (item.contains("actions")) {
      for (const auto& action : item["actions"]) {
        info.actions.push_back(parseAction(action));
      }
    }

    // Then another item's rules, e.g. an upgraded axe
    if (item.contains("actions_from")) {
      const auto& inherited = getInfo(item["actions_from"]).actions;
      info.actions.insert(info.actions.end(), inherited.begin(),
                          inherited.end());
    }

    item_info[id] = info;
  }

  // Close
  file.close();
  return 0;
}

// Returns item at ID
TileType& ItemTypeManager::getItem(const std::string& id) {
  if (!item_defs.contains(id)) {
    throw std::runtime_error("Item not found: " + id);
  }

  return item_defs[id];
}

const ItemInfo& ItemTypeManager::getInfo(const std::string& id) {
  if (!item_info.contains(id)) {
    throw std::runtime_error("Item not found: " + id);
  }

  return item_info[id];
}

bool ItemTypeManager::exists(const std::string& id) {
  return item_defs.contains(id);
}
