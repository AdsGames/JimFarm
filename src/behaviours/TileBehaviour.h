#ifndef TILE_BEHAVIOUR_H_
#define TILE_BEHAVIOUR_H_

#include <memory>

#include "../Item.h"
#include "../Tile.h"
#include "../TileMap.h"

class TileBehaviour {
 public:
  virtual ~TileBehaviour() = default;

  virtual void onInteract(std::shared_ptr<Item> item,
                          std::shared_ptr<Tile> tile,
                          TileMap& tile_map) = 0;
};

#endif  // TILE_BEHAVIOUR_H_