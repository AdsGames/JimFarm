#include "Tile.h"

#include <iostream>
#include <math.h>

#include "Tools.h"
#include "TileTypeManager.h"

#include "tile_defs.h"
#include "item_defs.h"

// Ctor for tile
Tile::Tile (char newID, int newX, int newY, unsigned char meta) {
  this -> x = newX;
  this -> y = newY;
  this -> tile_pointer = TileTypeManager::getTileByID (newID);
  this -> meta = meta;
}

// Draw tile to screen
void Tile::draw (BITMAP *tempBuffer) {
  tile_pointer -> draw (x, y, tempBuffer, getMeta());
}

// Draw tile at specific position
void Tile::draw_at (int newX, int newY, BITMAP *tempBuffer) {
  tile_pointer -> draw (newX, newY, tempBuffer);
}
