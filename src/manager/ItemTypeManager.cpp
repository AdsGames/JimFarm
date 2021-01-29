#include "ItemTypeManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../Tile.h"
#include "../utility/Tools.h"

std::vector<TileType> ItemTypeManager::item_defs;

BITMAP* ItemTypeManager::sprite_sheet_items = NULL;

// Destructor
ItemTypeManager::~ItemTypeManager() {
  item_defs.clear();
}

// Load tiles
int ItemTypeManager::loadItems(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (auto const& item : doc) {
    // Name of item
    std::string name = item["name"];

    // Spritesheet info
    int image_x = item["image_x"];

    int image_y = item["image_y"];

    // Cost if sold
    int value = item["value"];

    int id = item["id"];

    // Create item, set variables and add it to the item list
    TileType newTileType(1, 1, id, name, 0, (unsigned char)value);
    newTileType.setSpriteSheet(sprite_sheet_items);
    newTileType.setImageType("", 1, 1, image_x, image_y, 1, 1);
    item_defs.push_back(newTileType);
  }

  // Close
  file.close();
  return 0;
}

// Returns item at ID
TileType* ItemTypeManager::getItemById(int tileID) {
  for (auto& item : item_defs) {
    if (item.getID() == tileID) {
      return &item;
    }
  }
  return nullptr;
}
