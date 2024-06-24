#ifndef TREE_BEHAVIOUR_H_
#define TREE_BEHAVIOUR_H_

#include "TileBehaviour.h"

class TreeBehaviour : public TileBehaviour {
 public:
  void onInteract(std::shared_ptr<Item> item,
                  std::shared_ptr<Tile> tile,
                  TileMap& tile_map) override;
};

#endif  // TREE_BEHAVIOUR_H_