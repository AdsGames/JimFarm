#include "tile.h"

#include <iostream>
#include <math.h>

#include "tools.h"
#include "tile_type_manager.h"
#include "tile_defs.h"
#include "item_defs.h"

// TILE
tile::tile(int newX, int newY, char newID){
  // Set init variables
  this -> x = newX;
  this -> y = newY;

  this -> tile_pointer = tile_type_manager::getTileByID( newID);

  this -> meta = 0;
}

tile::~tile(){}

void tile::draw( BITMAP *tempBuffer){
  tile_pointer -> draw( x, y, tempBuffer, getMeta());
}

void tile::draw_at( int newX, int newY, BITMAP *tempBuffer){
  tile_pointer -> draw( newX, newY, tempBuffer);
}

void tile::setID( unsigned char newID){
  tile_pointer = tile_type_manager::getTileByID( newID);
}
