#include "ItemTypeManager.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Tile.h"
#include "../utility/Tools.h"

std::map<std::string, TileType> ItemTypeManager::item_defs;

asw::Texture ItemTypeManager::sprite_sheet_items = nullptr;

// Load tiles
int ItemTypeManager::loadItems(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Parse data
  for (auto const& item : nlohmann::json::parse(file)) {
    // Read from json
    std::string name = item["name"];
    unsigned char image_x = item["image_x"];
    unsigned char image_y = item["image_y"];
    std::string id = item["id"];

    // Create item, set variables and add it to the item list
    item_defs[id] = TileType(1, 1, id, name, 0);
    item_defs[id].setSpriteSheet(sprite_sheet_items);
    item_defs[id].setImageType("", 1, 1, image_x, image_y, 1, 1);
  }

  // Close
  file.close();
  return 0;
}

// Returns item at ID
TileType& ItemTypeManager::getItem(const std::string& id) {
  if (!item_defs.contains(id)) {
    throw std::runtime_error("Item not found");
  }

  return item_defs[id];
}
