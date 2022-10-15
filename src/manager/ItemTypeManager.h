/*
  Item Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of items into a container for access by item objects.
*/
#ifndef ITEM_TYPE_MANAGER_H
#define ITEM_TYPE_MANAGER_H

#define NON_SOLID 0
#define SOLID 1

#include <string>
#include <vector>

#include "../ui/UI_Controller.h"
#include "TileType.h"

class ItemTypeManager {
 public:
  ItemTypeManager(){};
  ~ItemTypeManager();

  // Load tile types
  static int loadItems(std::string path);

  // Allows communication
  static TileType* getItemById(int tileID);

  static asw::Texture sprite_sheet_items;

 private:
  // Stores all tiles and items
  static std::vector<TileType> item_defs;
};

#endif  // ITEM_TYPE_MANAGER_H
