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
int ItemTypeManager::load_items(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (unsigned int i = 0; i < doc.size(); i++) {
    // Name of item
    std::string name = doc[i]["name"];

    // Spritesheet info
    int image_x = doc[i]["image_x"];

    int image_y = doc[i]["image_y"];

    // Cost if sold
    int value = doc[i]["value"];

    // Create item, set variables and add it to the item list
    TileType newTileType(1, 1, i, name, 0, (unsigned char)value);
    newTileType.setSpriteSheet(sprite_sheet_items);
    newTileType.setImageType("", 1, 1, image_x, image_y, 1, 1);
    item_defs.push_back(newTileType);
  }

  // Close
  file.close();
  return 0;
}

// Returns item at ID
TileType* ItemTypeManager::getItemByID(int tileID) {
  for (unsigned int i = 0; i < item_defs.size(); i++) {
    if (item_defs.at(i).getID() == tileID) {
      return &item_defs.at(i);
    }
  }
  return nullptr;
}
