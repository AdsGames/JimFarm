#include "FarmBehaviours.h"

#include <algorithm>
#include <array>
#include <format>

#include "../World.h"
#include "CraftBehaviours.h"
#include "../manager/SoundManager.h"
#include "../utility/Tools.h"

namespace {
bool contains(const std::vector<std::string>& list, const std::string& value) {
  return std::ranges::find(list, value) != list.end();
}

std::string heldId(const ItemStack& held) {
  return held.getItem() ? held.getItem()->getType().getId() : "";
}

// Crows only come once the farm is going
constexpr int FIRST_CROW_DAY = 6;
constexpr int MAX_SCARECROW_RADIUS = 6;
constexpr int MAX_HOPPER_RADIUS = 6;

template <typename T>
const T* behaviourOf(const Tile& tile) {
  for (auto const& behaviour : tile.getType().getBehaviours()) {
    if (auto found = dynamic_cast<const T*>(behaviour.get())) {
      return found;
    }
  }
  return nullptr;
}

int tileDistance(const asw::Vec2i& a, const asw::Vec2i& b) {
  return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

// Scarecrow close enough to guard pos
bool guards(const std::shared_ptr<Tile>& tile, const asw::Vec2i& pos) {
  const auto* scarecrow = behaviourOf<ScarecrowBehaviour>(*tile);
  return scarecrow &&
         tileDistance(tile->getTilePosition(), pos) <= scarecrow->getRadius();
}
}  // namespace

/********
 * CROP *
 ********/
CropBehaviour::CropBehaviour(const nlohmann::json& params)
    : days(std::max(1, params.value("days", 4))),
      regrow(params.value("regrow", 0)),
      yield(params.value("yield", "")),
      yield_min(params.value("yield_min", 1)),
      yield_max(params.value("yield_max", 1)),
      needs_soil(params.value("needs_soil", "tile:watered_soil")),
      seasons(params.value("seasons", std::vector<std::string>{})),
      min_temp(params.value("min_temp", -64)),
      max_temp(params.value("max_temp", 64)),
      crow_chance(params.value("crow_chance", 0.04F)) {}

int CropBehaviour::daysGrown(const std::shared_ptr<Tile>& tile) const {
  // Meta holds progress 0-255, round up to whole days
  return (tile->getMeta() * days + MAX_TILE_META - 1) / MAX_TILE_META;
}

bool CropBehaviour::inSeason(const World& world) const {
  if (seasons.empty()) {
    return true;
  }

  const auto& state = world.getState();
  return contains(seasons, GameState::seasonName(state.getSeason()));
}

bool CropBehaviour::onInteract(World& world,
                               const std::shared_ptr<Tile>& tile,
                               ItemStack& held) {
  // Harvest
  if (tile->getMeta() >= MAX_TILE_META) {
    const auto tile_pos = tile->getTilePosition();
    world.dropItems(yield, tile_pos, random(yield_min, yield_max));
    world.getState().crops_harvested++;
    SoundManager::play("scythe");
    world.burstParticles("grass", tile_pos);

    if (regrow > 0 && regrow < days) {
      tile->setMeta(
          static_cast<unsigned char>((days - regrow) * MAX_TILE_META / days));
    } else {
      world.getMap().removeTile(tile);
    }

    return true;
  }

  // Empty hand shows how the crop is doing
  if (!held.getItem()) {
    const auto soil =
        world.getMap().getTileAt(tile->getTilePosition(), LAYER_MIDGROUND);
    const bool watered = soil && soil->getType().getId() == needs_soil;

    world.getState().notify(
        std::format("{}: day {}/{}{}", tile->getType().getName(),
                    daysGrown(tile), days, watered ? "" : ", needs water"));
    return true;
  }

  // Let the held item act on the soil (e.g. watering)
  return false;
}

void CropBehaviour::onDayEnd(World& world, const std::shared_ptr<Tile>& tile) {
  auto& map = world.getMap();
  const auto& state = world.getState();
  const auto tile_pos = tile->getTilePosition();

  // Out of season crops die
  if (!inSeason(world)) {
    map.removeTile(tile);
    world.countEvent("Crops withered (out of season)");
    return;
  }

  // Crows take unguarded crops
  if (state.absoluteDay() >= FIRST_CROW_DAY &&
      random(0, 999) < static_cast<int>(crow_chance * 1000.0F) &&
      !world.findTileNear(tile_pos, MAX_SCARECROW_RADIUS,
                          [&](const std::shared_ptr<Tile>& other) {
                            return guards(other, tile_pos);
                          })) {
    map.removeTile(tile);
    world.countEvent("Crops eaten by crows");
    return;
  }

  // Ripe crops wait for harvest
  if (tile->getMeta() >= MAX_TILE_META) {
    return;
  }

  // Needs water
  const auto soil = map.getTileAt(tile_pos, LAYER_MIDGROUND);
  if (!soil || soil->getType().getId() != needs_soil) {
    world.countEvent("Crops not watered");
    return;
  }

  // Biome must suit the crop
  const int temperature = map.getTemperatureAt(tile_pos);
  if (temperature < min_temp || temperature > max_temp) {
    world.countEvent(temperature < min_temp ? "Crops too cold to grow"
                                            : "Crops too hot to grow");
    return;
  }

  const int grown = daysGrown(tile) + 1;
  if (grown >= days) {
    tile->setMeta(MAX_TILE_META);
    world.countEvent("Crops ready to harvest");
  } else {
    tile->setMeta(static_cast<unsigned char>(grown * MAX_TILE_META / days));
  }
}

/*************
 * TRANSFORM *
 *************/
TransformBehaviour::TransformBehaviour(const nlohmann::json& params)
    : into(params.value("into", "")),
      days(params.value("days", 0)),
      chance(params.value("chance", 1.0F)),
      requires_empty_above(params.value("requires_empty_above", false)),
      meta_min(params.value("meta_min", 0)),
      meta_max(params.value("meta_max", 0)) {}

void TransformBehaviour::onDayEnd(World& world,
                                  const std::shared_ptr<Tile>& tile) {
  auto& map = world.getMap();

  if (requires_empty_above && tile->getZ() < LAYER_FOREGROUND &&
      map.getTileAt(tile->getTilePosition(), LAYER_FOREGROUND)) {
    return;
  }

  // Counts days in meta, only for tiles that do not bitmask
  if (days > 0) {
    tile->changeMeta(1);
    if (tile->getMeta() < days) {
      return;
    }
  } else if (random(0, 999) >= static_cast<int>(chance * 1000.0F)) {
    return;
  }

  map.replaceTile(tile, into,
                  static_cast<unsigned char>(random(meta_min, meta_max)));
}

/**********
 * ANIMAL *
 **********/
namespace {
// Meta layout: bit 0 fed today, bits 1-3 days without food
constexpr unsigned char FED_BIT = 1;

int hungryDays(unsigned char meta) {
  return (meta >> 1) & 0x07;
}

unsigned char makeMeta(bool fed, int hungry) {
  return static_cast<unsigned char>((fed ? FED_BIT : 0) |
                                    (std::min(hungry, 7) << 1));
}
}  // namespace

AnimalBehaviour::AnimalBehaviour(const nlohmann::json& params)
    : name(params.value("name", "animal")),
      product(params.value("product", "")),
      food(params.value("food", std::vector<std::string>{})),
      pickup(params.value("pickup", "")),
      wander(params.value("wander", 0.01F)),
      leave_days(params.value("leave_days", 3)) {}

bool AnimalBehaviour::onInteract(World& world,
                                 const std::shared_ptr<Tile>& tile,
                                 ItemStack& held) {
  const auto id = heldId(held);

  // Feed
  if (contains(food, id)) {
    if (tile->getMeta() & FED_BIT) {
      world.getState().notify("The " + name + " is full");
      return true;
    }

    held.remove(1);
    tile->setMeta(makeMeta(true, 0));
    SoundManager::play("egg");
    world.burstParticles("feathers", tile->getTilePosition());
    world.getState().notify("Fed the " + name);
    return true;
  }

  // Pick up with empty hand
  if (id.empty() && !pickup.empty()) {
    world.getMap().removeTile(tile);
    world.giveItem(pickup);
    return true;
  }

  return false;
}

void AnimalBehaviour::onTick(World& world, const std::shared_ptr<Tile>& tile) {
  if (random(0, 9999) >= static_cast<int>(wander * 10000.0F)) {
    return;
  }

  const std::array<asw::Vec2i, 4> directions = {
      asw::Vec2i(0, -1), asw::Vec2i(1, 0), asw::Vec2i(0, 1), asw::Vec2i(-1, 0)};

  auto& map = world.getMap();
  const auto to = tile->getTilePosition() + directions[random(0, 3)];

  // Walk onto open ground only
  if (to == world.getPlayerTile() || map.getTileAt(to, LAYER_FOREGROUND) ||
      !map.getTileAt(to, LAYER_MIDGROUND)) {
    return;
  }

  const auto meta = tile->getMeta();
  const auto id = tile->getType().getId();
  map.removeTile(tile);
  map.placeTile(
      std::make_shared<Tile>(id, to * TILE_SIZE, LAYER_FOREGROUND, meta));
}

void AnimalBehaviour::onDayEnd(World& world,
                               const std::shared_ptr<Tile>& tile) {
  const auto meta = tile->getMeta();

  if (meta & FED_BIT) {
    if (!product.empty()) {
      const auto pos = tile->getTilePosition();

      // A hopper nearby collects it, else it lands next to the animal
      const auto hopper = world.findTileNear(
          pos, MAX_HOPPER_RADIUS, [&](const std::shared_ptr<Tile>& other) {
            const auto* found = behaviourOf<HopperBehaviour>(*other);
            return found &&
                   tileDistance(other->getTilePosition(), pos) <=
                       found->getRadius() &&
                   found->store(other, product);
          });

      if (!hopper) {
        world.dropNear(product, pos);
      }
      world.countEvent("Animal products");
    }
    tile->setMeta(makeMeta(false, 0));
    return;
  }

  const int hungry = hungryDays(meta) + 1;
  if (hungry >= leave_days) {
    world.getMap().removeTile(tile);
    world.countEvent("Hungry animals ran away");
    return;
  }

  world.countEvent("Animals not fed");
  tile->setMeta(makeMeta(false, hungry));
}

/**********
 * FORAGE *
 **********/
ForageBehaviour::ForageBehaviour(const nlohmann::json& params)
    : yield(params.value("yield", "")),
      yield_min(params.value("yield_min", 1)),
      yield_max(params.value("yield_max", 1)),
      regrow(params.value("regrow", 3)) {}

bool ForageBehaviour::onInteract(World& world,
                                 const std::shared_ptr<Tile>& tile,
                                 ItemStack& held) {
  // Tools act on the tile as usual
  if (held.getItem()) {
    return false;
  }

  if (tile->getMeta() > 0) {
    world.getState().notify(
        std::format("Nothing to pick, back in {} days", tile->getMeta()));
    return true;
  }

  world.dropItems(yield, tile->getTilePosition(), random(yield_min, yield_max));
  tile->setMeta(static_cast<unsigned char>(regrow));
  SoundManager::play("scythe");
  world.burstParticles("leaves", tile->getTilePosition());
  return true;
}

void ForageBehaviour::onDayEnd(World& world,
                               const std::shared_ptr<Tile>& tile) {
  (void)world;
  if (tile->getMeta() > 0) {
    tile->setMeta(tile->getMeta() - 1);
  }
}
