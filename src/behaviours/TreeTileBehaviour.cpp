#include "TreeTileBehaviour.h"

void TreeBehaviour::onInteract(std::shared_ptr<Item> item,
                               std::shared_ptr<Tile> tile,
                               TileMap& tile_map) {
  // Check if item is axe
  if (item->getType().getId() == "item:axe") {
    // Get tile position
    auto tile_pos = tile->getPosition();
    auto inter_pos = tile->getTilePosition();

    // Cut down tree
    tile_map.removeTile(tile);
    tile_map.placeTile(
        std::make_shared<Tile>("tile:stump", inter_pos, LAYER_FOREGROUND));
    tile_map.placeItemAt(std::make_shared<Item>("item:stick", 0), tile_pos);
    tile_map.placeItemAt(std::make_shared<Item>("item:stick", 0), tile_pos);
    tile_map.placeItemAt(std::make_shared<Item>("item:wood", 0), tile_pos);
  }
};