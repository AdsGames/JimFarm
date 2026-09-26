#include "BuildingBehaviours.h"

#include <algorithm>
#include <format>

#include "../World.h"
#include "../manager/SoundManager.h"
#include "../utility/Tools.h"

// Ticks per fuel unit on average, 20 ticks a second and 4 game minutes a
// second, so one unit is about 5 game minutes
constexpr int TICKS_PER_FUEL = 25;

/********
 * SHOP *
 ********/
bool ShopBehaviour::onInteract(World& world,
                               const std::shared_ptr<Tile>& tile,
                               ItemStack& held) {
  (void)tile;
  (void)held;
  world.openShop();
  return true;
}

std::string ShopBehaviour::interactVerb(World& world,
                                        const std::shared_ptr<Tile>& tile,
                                        const ItemStack& held) {
  (void)world;
  (void)tile;
  (void)held;
  return "Trade";
}

/*******
 * BED *
 *******/
bool BedBehaviour::onInteract(World& world,
                              const std::shared_ptr<Tile>& tile,
                              ItemStack& held) {
  (void)held;

  // Wake up next to the last bed slept in
  auto& state = world.getState();
  const auto home = world.openTileNear(tile->getTilePosition());
  if (home != state.home) {
    state.home = home;
    state.notify("You will wake up here");
  }

  state.sleep_requested = true;
  return true;
}

std::string BedBehaviour::interactVerb(World& world,
                                       const std::shared_ptr<Tile>& tile,
                                       const ItemStack& held) {
  (void)world;
  (void)tile;
  (void)held;
  return "Sleep";
}

/************
 * CAMPFIRE *
 ************/
CampfireBehaviour::CampfireBehaviour(const nlohmann::json& params)
    : lit(params.value("lit", true)),
      lit_tile(params.value("lit_tile", "tile:campfire")),
      out_tile(params.value("out_tile", "tile:campfire_out")),
      fuel(params.value(
          "fuel",
          std::map<std::string, int>{{"item:wood", 48}, {"item:stick", 12}})),
      overnight_burn(params.value("overnight_burn", 100)) {}

bool CampfireBehaviour::isFuel(const ItemStack& held) const {
  return held.getItem() && fuel.contains(held.getItem()->getType().getId());
}

bool CampfireBehaviour::onUse(World& world,
                              const std::shared_ptr<Tile>& tile,
                              ItemStack& held) {
  // Fuel either way, other items cook by their own rules
  return addFuel(world, tile, held);
}

std::string CampfireBehaviour::useVerb(World& world,
                                       const std::shared_ptr<Tile>& tile,
                                       const ItemStack& held) {
  (void)world;
  (void)tile;
  return isFuel(held) ? (lit ? "Add fuel" : "Light") : "";
}

std::string CampfireBehaviour::interactVerb(World& world,
                                            const std::shared_ptr<Tile>& tile,
                                            const ItemStack& held) {
  (void)world;
  (void)tile;
  if (isFuel(held)) {
    return lit ? "Add fuel" : "Light";
  }
  return "Check fire";
}

bool CampfireBehaviour::addFuel(World& world,
                                const std::shared_ptr<Tile>& tile,
                                ItemStack& held) {
  if (!isFuel(held)) {
    return false;
  }

  const auto item = held.getItem();
  const int amount = fuel.at(item->getType().getId());
  const int total = std::min<int>(MAX_TILE_META, tile->getMeta() + amount);
  held.remove(1);

  if (lit) {
    tile->setMeta(static_cast<unsigned char>(total));
  } else {
    world.getMap().replaceTile(tile, lit_tile,
                               static_cast<unsigned char>(total));
    world.getState().notify("The fire is lit");
  }

  SoundManager::play("shovel");
  world.burstParticles("sparks", tile->getTilePosition());
  return true;
}

bool CampfireBehaviour::onInteract(World& world,
                                   const std::shared_ptr<Tile>& tile,
                                   ItemStack& held) {
  if (addFuel(world, tile, held)) {
    return true;
  }

  // Anything else checks the fuel
  const int hours = tile->getMeta() * TICKS_PER_FUEL / 20 *
                    static_cast<int>(GAME_MINUTES_PER_SECOND) / 60;
  world.getState().notify(
      lit ? std::format("The fire will burn about {} more hours", hours)
          : "Add wood or sticks to light the fire");
  return true;
}

void CampfireBehaviour::onTick(World& world,
                               const std::shared_ptr<Tile>& tile) {
  const auto tile_pos = tile->getTilePosition();
  world.addLight(tile_pos);

  // About 5 puffs of smoke and 2 sparks a second, at 20 ticks a second
  if (random(0, 3) == 0) {
    world.burstParticles("fire_smoke", tile_pos);
  }
  if (random(0, 7) == 0) {
    world.burstParticles("fire_spark", tile_pos);
  }

  if (random(0, TICKS_PER_FUEL - 1) != 0) {
    return;
  }

  if (tile->getMeta() == 0) {
    world.getMap().replaceTile(tile, out_tile);
    world.burstParticles("smoke", tile_pos);
    return;
  }

  tile->setMeta(tile->getMeta() - 1);
}

void CampfireBehaviour::onDayEnd(World& world,
                                 const std::shared_ptr<Tile>& tile) {
  if (!lit) {
    return;
  }

  // Burns through the night while the player sleeps
  if (tile->getMeta() <= overnight_burn) {
    world.getMap().replaceTile(tile, out_tile);
    return;
  }

  tile->setMeta(static_cast<unsigned char>(tile->getMeta() - overnight_burn));
}

/********
 * DOOR *
 ********/
namespace {
constexpr unsigned char DOOR_HELD_OPEN = 1;
}  // namespace

DoorBehaviour::DoorBehaviour(const nlohmann::json& params)
    : open(params.value("open", false)),
      open_tile(params.value("open_tile", "tile:door_open")),
      closed_tile(params.value("closed_tile", "tile:door")) {}

bool DoorBehaviour::close(World& world,
                          const std::shared_ptr<Tile>& tile) const {
  const auto at = tile->getTilePosition();
  if (world.getPlayerTile() == at || world.creatureAt(at)) {
    return false;
  }

  world.getMap().replaceTile(tile, closed_tile);
  return true;
}

bool DoorBehaviour::onInteract(World& world,
                               const std::shared_ptr<Tile>& tile,
                               ItemStack& held) {
  (void)held;

  if (!open) {
    world.getMap().replaceTile(tile, open_tile, DOOR_HELD_OPEN);
    SoundManager::play("shovel");
    return true;
  }

  if (!close(world, tile)) {
    world.getState().notify("Something is in the doorway");
    SoundManager::play("error");
    return true;
  }

  SoundManager::play("shovel");
  return true;
}

std::string DoorBehaviour::interactVerb(World& world,
                                        const std::shared_ptr<Tile>& tile,
                                        const ItemStack& held) {
  (void)world;
  (void)tile;
  (void)held;
  return open ? "Close" : "Hold open";
}

void DoorBehaviour::onTick(World& world, const std::shared_ptr<Tile>& tile) {
  // Swings shut behind the player unless held open
  if (tile->getMeta() != DOOR_HELD_OPEN) {
    close(world, tile);
  }
}
