/*
  Tile Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of tiles into a container for access by tile objects.
*/
#ifndef TILE_TYPE_MANAGER_H
#define TILE_TYPE_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "../ui/UI_Controller.h"
#include "TileType.h"

class TileTypeManager {
 public:
  // Load tile types
  static int loadTiles(const std::string& path);

  // Allows communication
  static std::shared_ptr<TileType> getTileById(int tileID);

  static asw::Texture sprite_sheet_tiles;
  static asw::Texture sprite_sheet_items;

 private:
  static std::vector<std::shared_ptr<TileType>> tile_defs;
};

#endif  // TILE_TYPE_MANAGER_H
