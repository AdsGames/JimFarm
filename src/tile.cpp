#include "Tile.h"

#include <iostream>
#include <math.h>

#include "Tools.h"
#include "TileTypeManager.h"

#include "tile_defs.h"
#include "item_defs.h"

// Ctor for tile
Tile::Tile (char id, int x, int y, int z, unsigned char meta) :
  Sprite(x, y, z) {
  this -> x = x;
  this -> y = y;
  this -> tile_pointer = TileTypeManager::getTileByID (id);
  this -> meta = meta;
}

// Draw tile to screen
void Tile::draw (BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2) {
  if (x >= x_1 - tile_pointer -> getWidth() &&
      x <= x_2 + tile_pointer -> getWidth() &&
      y >= y_1 - tile_pointer -> getHeight() &&
      y <= y_2 + tile_pointer -> getHeight()) {
    tile_pointer -> draw (x - x_1, y - y_1, tempBuffer, getMeta());
  }
}

// Draw tile at specific position
void Tile::draw_at (int newX, int newY, BITMAP *tempBuffer) {
  tile_pointer -> draw (newX, newY, tempBuffer);
}
