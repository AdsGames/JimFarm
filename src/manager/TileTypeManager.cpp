#include "TileTypeManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Tile.h"
#include "../utility/Tools.h"

std::vector<TileType> TileTypeManager::tile_defs;

BITMAP* TileTypeManager::sprite_sheet_tiles = NULL;

// Destructor
TileTypeManager::~TileTypeManager() {
  tile_defs.clear();
}

// Load tiles
int TileTypeManager::loadTiles(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (auto const& tile : doc) {
    // Name
    std::string name = tile["name"];

    // ID value
    int id = tile["id"];

    // Spritesheet coordinates
    int image_x = tile["image"]["x"];
    int image_y = tile["image"]["y"];
    int image_h = tile["image"]["height"];
    int image_w = tile["image"]["width"];

    // Size
    int width = tile["width"];
    int height = tile["height"];

    // Special tile types
    int sheet_width = tile["image"]["spritesheet_width"];
    int sheet_height = tile["image"]["spritesheet_height"];
    std::string image_type = tile["image"]["type"];

    // Get attrubite
    int attrubite = NON_SOLID;
    if (tile["attribute"] == "SOLID")
      attrubite = SOLID;

    // Create tile, set variables and add it to the tile list
    TileType newTileType(width * 16, height * 16, id, name, attrubite);
    newTileType.setSpriteSheet(sprite_sheet_tiles);
    newTileType.setImageType(image_type, sheet_width, sheet_height, image_x,
                             image_y, image_w, image_h);
    tile_defs.push_back(newTileType);
  }

  // Close
  file.close();
  return 0;
}

// Returns tile at ID
TileType* TileTypeManager::getTileById(int tileID) {
  for (auto& tile : tile_defs) {
    if (tile.getID() == tileID) {
      return &tile;
    }
  }
  return nullptr;
}
