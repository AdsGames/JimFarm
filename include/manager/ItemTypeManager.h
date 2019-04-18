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

#include <vector>
#include <string>

#include "TileType.h"
#include "UI_Controller.h"

class ItemTypeManager {
  public:
    ItemTypeManager() {};
    ~ItemTypeManager();

    // Load tile types
    static int load_items (std::string path);

    // Allows communication
    static TileType *getItemByID (int tileID);

    static BITMAP *sprite_sheet_items;

  private:
    // Stores all tiles and items
    static std::vector<TileType> item_defs;
};

#endif // ITEM_TYPE_MANAGER_H
