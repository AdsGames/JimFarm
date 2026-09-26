#include "CraftBehaviours.h"

#include <algorithm>
#include <format>

#include "../World.h"
#include "../manager/ItemTypeManager.h"
#include "../manager/SoundManager.h"
#include "../utility/Tools.h"

/***********
 * STATION *
 ***********/
StationBehaviour::StationBehaviour(const nlohmann::json& params)
    : window(params.value("window", "crafting")),
      tier(params.value("tier", 0)) {}

bool StationBehaviour::onInteract(World& world,
                                  const std::shared_ptr<Tile>& tile,
                                  ItemStack& held) {
  // Tools pick the station up
  if (world.itemCanUse(held, tile->getTilePosition())) {
    return false;
  }

  world.openStation(window, tier);
  return true;
}

/*************
 * BREAKABLE *
 *************/
BreakableBehaviour::BreakableBehaviour(const nlohmann::json& params)
    : tools(params.value("tools", std::vector<std::string>{})),
      hits(std::max(1, params.value("hits", 3))),
      energy(params.value("energy", 3.0F)),
      into(params.value("into", "")),
      sound(params.value("sound", "axe")),
      hint(params.value("hint", "")) {
  const auto drop_list = params.value("drops", nlohmann::json::object());
  for (auto const& [id, range] : drop_list.items()) {
    drops[id] = {range[0].get<int>(), range[1].get<int>()};
  }
}

bool BreakableBehaviour::onInteract(World& world,
                                    const std::shared_ptr<Tile>& tile,
                                    ItemStack& held) {
  const auto item = held.getItem();
  if (!item) {
    return false;
  }

  const auto& id = item->getType().getId();
  const auto& info = ItemTypeManager::getInfo(id);

  if (std::ranges::find(tools, id) == tools.end()) {
    // A tool, but not a strong enough one
    if (info.power > 0.0F && !hint.empty()) {
      world.getState().notify(hint);
      SoundManager::play("error");
      return true;
    }
    return false;
  }

  if (!world.getState().useEnergy(energy)) {
    SoundManager::play("error");
    return true;
  }

  SoundManager::play(sound);

  const float power = std::max(1.0F, info.power);
  tile->damage(static_cast<unsigned char>(
      std::min(255.0F, (255.0F / hits + 1.0F) * power)));
  if (tile->getHitpoints() > 0) {
    return true;
  }

  const auto tile_pos = tile->getTilePosition();
  world.getMap().replaceTile(tile, into);

  for (auto const& [drop, range] : drops) {
    world.dropItems(drop, tile_pos, random(range.first, range.second));
  }

  return true;
}

/*************
 * SPRINKLER *
 *************/
SprinklerBehaviour::SprinklerBehaviour(const nlohmann::json& params)
    : radius(params.value("radius", 1)) {}

void SprinklerBehaviour::onMorning(World& world,
                                   const std::shared_ptr<Tile>& tile) {
  auto& map = world.getMap();

  for (int dx = -radius; dx <= radius; dx++) {
    for (int dy = -radius; dy <= radius; dy++) {
      const auto at = tile->getTilePosition() + asw::Vec2i(dx, dy);
      auto soil = map.getTileAt(at, LAYER_MIDGROUND);
      if (soil && soil->getType().getId() == "tile:plowed_soil") {
        map.replaceTile(soil, "tile:watered_soil");
      }
    }
  }
}

/*************
 * SCARECROW *
 *************/
ScarecrowBehaviour::ScarecrowBehaviour(const nlohmann::json& params)
    : radius(params.value("radius", 4)) {}

/**********
 * HOPPER *
 **********/
HopperBehaviour::HopperBehaviour(const nlohmann::json& params)
    : item(params.value("item", "item:egg")),
      radius(params.value("radius", 4)) {}

bool HopperBehaviour::onInteract(World& world,
                                 const std::shared_ptr<Tile>& tile,
                                 ItemStack& held) {
  // Tools pick the hopper up
  if (world.itemCanUse(held, tile->getTilePosition())) {
    return false;
  }

  const auto name = ItemTypeManager::getItem(item).getName();

  if (tile->getMeta() == 0) {
    world.getState().notify(std::format(
        "Empty. It collects {}s from animals within {} tiles", name, radius));
    return true;
  }

  world.giveItem(item, tile->getMeta());
  world.getState().notify(std::format("Took {} {}", tile->getMeta(), name));
  tile->setMeta(0);
  SoundManager::play("pickup");
  return true;
}

bool HopperBehaviour::store(const std::shared_ptr<Tile>& tile,
                            const std::string& id) const {
  if (id != item || tile->getMeta() >= MAX_TILE_META) {
    return false;
  }

  tile->setMeta(tile->getMeta() + 1);
  return true;
}
