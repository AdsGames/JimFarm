#include "Tile.h"

#include "utility/Tools.h"
#include "manager/TileTypeManager.h"

// Ctor for tile
Tile::Tile (char id, int x, int y, int z, unsigned char meta) :
  Sprite(x, y, z) {
  this -> x = x;
  this -> y = y;
  this -> tile_pointer = TileTypeManager::getTileByID (id);
  this -> meta = meta;
}

// Gets size
int Tile::getWidth() {
  return tile_pointer -> getWidth();
}

int Tile::getHeight() {
  return tile_pointer -> getHeight();
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

// Is this type solid?
bool Tile::isSolid() {
  return tile_pointer -> getAttribute();
}

// Modify ID
unsigned char Tile::getID() {
  return tile_pointer -> getID();
}

// Get name of tile
std::string Tile::getName() {
  return tile_pointer -> getName();
}

// Access and set meta data byte
void Tile::setMeta (unsigned char meta) {
  this -> meta = meta;
}

unsigned char Tile::getMeta() {
  return this -> meta;
}

void Tile::changeMeta (unsigned char amt) {
  this -> meta += amt;
}

// Get image type
bool Tile::needsBitmask() {
  return this -> tile_pointer -> getImageType() == "dynamic";
}
