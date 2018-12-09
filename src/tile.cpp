#include "Tile.h"

#include <iostream>
#include <math.h>

#include "Tools.h"
#include "TileTypeManager.h"

#include "tile_defs.h"
#include "item_defs.h"

// Ctor for tile
tile::tile (char newID, int newX, int newY, unsigned char meta) {
  this -> x = newX;
  this -> y = newY;
  this -> tile_pointer = tile_type_manager::getTileByID (newID);
  this -> meta = meta;
}

// Draw tile to screen
void tile::draw (BITMAP *tempBuffer) {
  tile_pointer -> draw (x, y, tempBuffer, getMeta());
}

// Draw tile at specific position
void tile::draw_at (int newX, int newY, BITMAP *tempBuffer) {
  tile_pointer -> draw (newX, newY, tempBuffer);
}
