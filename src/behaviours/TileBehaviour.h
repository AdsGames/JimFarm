#ifndef TILE_BEHAVIOUR_H_
#define TILE_BEHAVIOUR_H_

#include <memory>

#include "../ItemStack.h"
#include "../Tile.h"

class World;

class TileBehaviour {
 public:
  virtual ~TileBehaviour() = default;

  // Player uses held stack (may be empty) on the tile.
  // Return true when handled, which stops item actions from running.
  virtual bool onInteract(World& world,
                          const std::shared_ptr<Tile>& tile,
                          ItemStack& held) {
    (void)world;
    (void)tile;
    (void)held;
    return false;
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

  // Only ticking behaviours are visited every tick
  virtual bool ticks() const { return false; }
};

#endif  // TILE_BEHAVIOUR_H_
