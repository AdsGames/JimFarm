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
#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "tools.h"

#include "tile_type.h"

class tile_type_manager{
  public:
    tile_type_manager();
    virtual ~tile_type_manager();

    // Load tile types
    void load( std::string newFile);

    // Allows communication
    int getNumberTiles(){ return tile_defs.size(); }
    tile_type *getTileByType( int tileID);

  protected:
  private:
    std::vector<tile_type> tile_defs;
};

#endif // TILE_TYPE_MANAGER_H
