#ifndef TILE_BEHAVIOUR_H_
#define TILE_BEHAVIOUR_H_

#include <memory>
#include <string>

#include "../ItemStack.h"
#include "../Tile.h"

class World;

class TileBehaviour {
 public:
  virtual ~TileBehaviour() = default;

  // Left click: the player uses the held item on the tile, e.g. an axe on a
  // tree. Return true when handled, which stops item actions from running.
  virtual bool onUse(World& world,
                     const std::shared_ptr<Tile>& tile,
                     ItemStack& held) {
    (void)world;
    (void)tile;
    (void)held;
    return false;
  }

  // Right click: the player interacts with the tile, e.g. opens, harvests or
  // feeds it. The held stack may be empty. Return true when handled.
  virtual bool onInteract(World& world,
                          const std::shared_ptr<Tile>& tile,
                          ItemStack& held) {
    (void)world;
    (void)tile;
    (void)held;
    return false;
  }

  // Words shown under the cursor for left and right click, empty when the
  // click does nothing here
  virtual std::string useVerb(World& world,
                              const std::shared_ptr<Tile>& tile,
                              const ItemStack& held) {
    (void)world;
    (void)tile;
    (void)held;
    return "";
  }

  virtual std::string interactVerb(World& world,
                                   const std::shared_ptr<Tile>& tile,
                                   const ItemStack& held) {
    (void)world;
    (void)tile;
    (void)held;
    return "";
  }

  // Called each game tick (20 per second) while the tile is near the camera
  virtual void onTick(World& world, const std::shared_ptr<Tile>& tile) {
    (void)world;
    (void)tile;
  }

  // Called for every tile in the world when the day ends
  virtual void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) {
    (void)world;
    (void)tile;
  }

  // Called for every tile after all day end behaviours ran, e.g. sprinklers
  // water soil after it dried
  virtual void onMorning(World& world, const std::shared_ptr<Tile>& tile) {
    (void)world;
    (void)tile;
  }

  // Only ticking behaviours are visited every tick
  virtual bool ticks() const { return false; }
};

#endif  // TILE_BEHAVIOUR_H_
