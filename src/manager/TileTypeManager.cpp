#include "TileTypeManager.h"

#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Tile.h"
#include "../utility/Tools.h"

const int NON_SOLID = 0;
const int SOLID = 1;

std::map<std::string, TileType> TileTypeManager::tile_defs{};

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
    std::string id = tile["id"];

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
    unsigned char attribute = NON_SOLID;
    if (tile["attribute"] == "SOLID") {
      attribute = SOLID;
    }

    // Create tile, set variables and add it to the tile list
    tile_defs[id] = TileType(width * 16, height * 16, id, name, attribute);
    tile_defs[id].setSpriteSheet(sprite_sheet_tiles);
    tile_defs[id].setImageType(image_type, sheet_width, sheet_height, image_x,
                               image_y, image_w, image_h);
  }

  // Close
  file.close();
  return 0;
}

// Returns tile at ID
TileType& TileTypeManager::getTile(const std::string& id) {
  if (!tile_defs.contains(id)) {
    throw std::runtime_error("Tile not found with id: " + id + ".");
  }

  return tile_defs[id];
}
