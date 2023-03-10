#include "TileTypeManager.h"

#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Tile.h"
#include "../utility/Tools.h"

const int NON_SOLID = 0;
const int SOLID = 1;

std::vector<TileType> TileTypeManager::tile_defs;

asw::Texture TileTypeManager::sprite_sheet_tiles = nullptr;

// Load tiles
int TileTypeManager::loadTiles(const std::string& path) {
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
    unsigned char image_x = tile["image"]["x"];
    unsigned char image_y = tile["image"]["y"];
    unsigned char image_h = tile["image"]["height"];
    unsigned char image_w = tile["image"]["width"];

    // Size
    unsigned char width = tile["width"];
    unsigned char height = tile["height"];

    // Special tile types
    unsigned char sheet_width = tile["image"]["spritesheet_width"];
    unsigned char sheet_height = tile["image"]["spritesheet_height"];
    std::string image_type = tile["image"]["type"];

    // Get attrubite
    int attrubite = NON_SOLID;
    if (tile["attribute"] == "SOLID")
      attrubite = SOLID;

    // Create tile, set variables and add it to the tile list
    auto tile_type = TileType(width * 16, height * 16, id, name, attrubite);
    tile_type.setSpriteSheet(sprite_sheet_tiles);
    tile_type.setImageType(image_type, sheet_width, sheet_height, image_x,
                           image_y, image_w, image_h);
    tile_defs.push_back(tile_type);
  }

  // Close
  file.close();
  return 0;
}

// Returns tile at ID
TileType& TileTypeManager::getTileById(int tileID) {
  for (auto& tile : tile_defs) {
    if (tile.getId() == tileID) {
      return tile;
    }
  }

  // Throw error if tile not found
  throw std::runtime_error("Tile not found");
}
