/*
  Item Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of items into a container for access by item objects.
*/
#ifndef ITEM_TYPE_MANAGER_H
#define ITEM_TYPE_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "../ui/UiController.h"
#include "TileType.h"

const int NON_SOLID = 0;
const int SOLID = 1;

class ItemTypeManager {
 public:
  // Load tile types
  static int loadItems(const std::string& path);

  // Allows communication
  static TileType& getItemById(int tileID);

  static asw::Texture sprite_sheet_items;

 private:
  // Stores all tiles and items
  static std::vector<TileType> item_defs;
};

#endif  // ITEM_TYPE_MANAGER_H
