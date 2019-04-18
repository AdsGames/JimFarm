#include "ItemTypeManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "Tools.h"
#include "Tile.h"

std::vector<TileType> ItemTypeManager::item_defs;

BITMAP *ItemTypeManager::sprite_sheet_items = NULL;

// Destructor
ItemTypeManager::~ItemTypeManager() {
  item_defs.clear();
}

// Load tiles
int ItemTypeManager::load_items (std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path.c_str());
  if (!file)
    return 1;

  // Create buffer
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());

  // Get first node
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);
  rapidxml::xml_node<> *allItems = doc.first_node();

  // Define iterator
  rapidxml::xml_node<> *cItem;
  cItem = allItems -> first_node("item");

  // Parse data
  for (int i = 0; cItem != NULL; cItem = cItem -> next_sibling(), i++) {
    // Name of item
    std::string name = cItem -> first_node("name") -> value();

    // Spritesheet info
    int image_x = atoi(cItem -> first_node("image") -> first_attribute("x") -> value());
    int image_y = atoi(cItem -> first_node("image") -> first_attribute("y") -> value());

    // Cost if sold
    int value = atoi(cItem -> first_node("value") -> value());

    // Create item, set variables and add it to the item list
    TileType newTileType (1, 1, i, name, 0, (unsigned char)value);
    newTileType.setSpriteSheet (sprite_sheet_items);
    newTileType.setImageType ("", 1, 1, image_x, image_y, 1, 1);
    item_defs.push_back (newTileType);
  }

  // Close
  file.close();
  return 0;
}

// Returns item at ID
TileType *ItemTypeManager::getItemByID (int tileID) {
  for (unsigned int i = 0; i < item_defs.size(); i++) {
    if (item_defs.at (i).getID() == tileID) {
      return &item_defs.at (i);
    }
  }
  return nullptr;
}
