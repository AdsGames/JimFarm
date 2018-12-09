/*
  Tile Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of tiles into a container for access by tile objects.
*/
#ifndef TILE_TYPE_MANAGER_H
#define TILE_TYPE_MANAGER_H

#define NON_SOLID 0
#define SOLID 1

#include <vector>
#include <string>

#include "TileType.h"

class TileTypeManager {
  public:
    TileTypeManager() {};
    ~TileTypeManager();

    // Load tile types
    static int load_tiles (std::string newFile);
    static int load_items (std::string newFile);

    // Allows communication
    static TileType *getTileByID (int tileID);
    static TileType *getItemByID (int tileID);

    static BITMAP *sprite_sheet_tiles;
    static BITMAP *sprite_sheet_items;

  private:
    // Stores all tiles and items
    static std::vector<TileType> tile_defs;
    static std::vector<TileType> item_defs;
};

#endif // TILE_TYPE_MANAGER_H
