#include "TileTypeManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "Tools.h"
#include "Tile.h"

std::vector<TileType> TileTypeManager::tile_defs;
std::vector<TileType> TileTypeManager::item_defs;

BITMAP *TileTypeManager::sprite_sheet_tiles = NULL;
BITMAP *TileTypeManager::sprite_sheet_items = NULL;

// Destructor
TileTypeManager::~TileTypeManager() {
  item_defs.clear();
  tile_defs.clear();
}

// Load tiles
int TileTypeManager::load_tiles (std::string newFile) {
  // Open file or abort if it does not exist
  std::ifstream file(newFile.c_str());
  if (!file)
    return 1;

  // Create buffer
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());

  // Get first node
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);
  rapidxml::xml_node<> *allTiles = doc.first_node();

  // Define iterator
  rapidxml::xml_node<> *cTile;
  cTile = allTiles -> first_node("tile");

  // Parse data
  for (; cTile != NULL; cTile = cTile -> next_sibling()) {
    // Name
    std::string name = cTile -> first_node("name") -> value();

    // ID value
    int id = atoi(cTile -> first_attribute("id") -> value());

    // Spritesheet coordinates
    int image_x = atoi (cTile -> first_node("image") -> first_attribute("x") -> value());
    int image_y = atoi (cTile -> first_node("image") -> first_attribute("y") -> value());
    int image_h = atoi (cTile -> first_node("image") -> first_attribute("h") -> value());
    int image_w = atoi (cTile -> first_node("image") -> first_attribute("w") -> value());

    // Size
    int width  = atoi (cTile -> first_node("width" ) -> value());
    int height = atoi (cTile -> first_node("height") -> value());

    // Special tile types
    int sheet_width  = atoi (cTile-> first_node("image") -> first_attribute("s_w") -> value());
    int sheet_height = atoi (cTile-> first_node("image") -> first_attribute("s_h") -> value());
    std::string image_type = cTile-> first_node("image") -> first_attribute("type") -> value();

    // Get attrubite
    std::string attrubite_string = cTile-> first_node("attrubite") -> value();
    int attrubite = NON_SOLID;
    if (attrubite_string == "NON_SOLID")
      attrubite = NON_SOLID;
    else if (attrubite_string == "SOLID")
      attrubite = SOLID;

    // Create tile, set variables and add it to the tile list
    TileType newTileType (width * 16, height * 16, id, name, attrubite);
    newTileType.setSpriteSheet (sprite_sheet_tiles);
    newTileType.setImageType (image_type, sheet_width, sheet_height, image_x, image_y, image_w, image_h);
    tile_defs.push_back (newTileType);
  }

  // Close
  file.close();
  return 0;
}

// Load tiles
int TileTypeManager::load_items (std::string newFile) {
  // Open file or abort if it does not exist
  std::ifstream file(newFile.c_str());
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
    std::string name = cItem-> first_node("name") -> value();

    // Spritesheet info
    int image_x = atoi(cItem-> first_node("image") -> first_attribute("x") -> value());
    int image_y = atoi(cItem-> first_node("image") -> first_attribute("y") -> value());

    // Cost if sold
    int value = atoi(cItem-> first_node("value") -> value());

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

// Returns tile at ID
TileType *TileTypeManager::getTileByID (int tileID) {
  for (unsigned int i = 0; i < tile_defs.size(); i++) {
    if (tile_defs.at (i).getID() == tileID) {
      return &tile_defs.at (i);
    }
  }
  return nullptr;
}

// Returns item at ID
TileType *TileTypeManager::getItemByID (int tileID) {
  for (unsigned int i = 0; i < item_defs.size(); i++) {
    if (item_defs.at (i).getID() == tileID) {
      return &item_defs.at (i);
    }
  }
  return nullptr;
}
