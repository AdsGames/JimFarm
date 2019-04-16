#include "TileTypeManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "Tools.h"
#include "Tile.h"

#include "UI_Label.h"
#include "UI_Slot.h"

std::vector<TileType> TileTypeManager::tile_defs;
std::vector<TileType> TileTypeManager::item_defs;
std::vector<UI_Controller*> TileTypeManager::ui_defs;

BITMAP *TileTypeManager::sprite_sheet_tiles = NULL;
BITMAP *TileTypeManager::sprite_sheet_items = NULL;

// Destructor
TileTypeManager::~TileTypeManager() {
  item_defs.clear();
  tile_defs.clear();
}

// Load tiles
int TileTypeManager::load_tiles (std::string path) {
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
int TileTypeManager::load_items (std::string path) {
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

// Load interfaces
int TileTypeManager::load_interfaces(std::string path) {
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
  rapidxml::xml_node<> *allInterfaces = doc.first_node();

  // Parse data
  for (rapidxml::xml_node<> *cInterface = allInterfaces -> first_node("interface");
       cInterface != nullptr;
       cInterface = cInterface -> next_sibling("interface")) {

    // Name of interface
    std::string name = cInterface-> first_node("name") -> value();

    // Width and Height
    int width = atoi(cInterface -> first_attribute("width") -> value());
    int height = atoi(cInterface -> first_attribute("height") -> value());

    // Create ui controller
    UI_Controller *controller = new UI_Controller(width, height);

    // Labels
    for (rapidxml::xml_node<> *cLabel = cInterface -> first_node("label"); cLabel != nullptr; cLabel = cLabel -> next_sibling("label")) {
      std::string text = cLabel-> first_attribute("text") -> value();
      int x = atoi(cLabel -> first_attribute("x") -> value());
      int y = atoi(cLabel -> first_attribute("y") -> value());
      controller -> AddElement(new UI_Label(x, y, text));
    }

    // Slots
    for (rapidxml::xml_node<> *cSlot = cInterface -> first_node("slot"); cSlot != nullptr; cSlot = cSlot -> next_sibling("slot")) {
      int x = atoi(cSlot -> first_attribute("x") -> value());
      int y = atoi(cSlot -> first_attribute("y") -> value());
      controller -> AddElement(new UI_Slot(x, y));
    }

    // Push to controllers
    ui_defs.push_back (controller);
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

UI_Controller* TileTypeManager::getInterfaceByID (int id) {
  if (id >= 0 && id < (signed)ui_defs.size())
    return ui_defs.at(id);
  return nullptr;
}
