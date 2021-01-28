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
int TileTypeManager::load_tiles(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (unsigned int i = 0; i < doc.size(); i++) {
    // Name
    std::string name = doc[i]["name"];

    // ID value
    int id = doc[i]["id"];

    // Spritesheet coordinates
    int image_x = doc[i]["image"]["x"];
    int image_y = doc[i]["image"]["y"];
    int image_h = doc[i]["image"]["height"];
    int image_w = doc[i]["image"]["width"];

    // Size
    int width = doc[i]["width"];
    int height = doc[i]["height"];

    // Special tile types
    int sheet_width = doc[i]["image"]["spritesheet_width"];
    int sheet_height = doc[i]["image"]["spritesheet_height"];
    std::string image_type = doc[i]["image"]["type"];

    // Get attrubite
    int attrubite = NON_SOLID;
    if (doc[i]["attribute"] == "SOLID")
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
TileType* TileTypeManager::getTileByID(int tileID) {
  for (unsigned int i = 0; i < tile_defs.size(); i++) {
    if (tile_defs.at(i).getID() == tileID) {
      return &tile_defs.at(i);
    }
  }
  return nullptr;
}
