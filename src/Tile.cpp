#include "Tile.h"

#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

// Ctor for tile
Tile::Tile(char id, int x, int y, int z, unsigned char meta)
    : Sprite(x, y, z),
      meta(meta),
      tile_pointer(TileTypeManager::getTileById(id)) {}

// Gets size
int Tile::getWidth() const {
  return tile_pointer.getWidth();
}

int Tile::getHeight() const {
  return tile_pointer.getHeight();
}

// Draw tile to screen
void Tile::draw(float x_1, float y_1, float x_2, float y_2) const {
  if (x >= x_1 - tile_pointer.getWidth() &&
      x <= x_2 + tile_pointer.getWidth() &&
      y >= y_1 - tile_pointer.getHeight() &&
      y <= y_2 + tile_pointer.getHeight()) {
    tile_pointer.draw(x - x_1, y - y_1, getMeta());
  }
}

// Draw tile at specific position
void Tile::drawAt(int newX, int newY) const {
  tile_pointer.draw(newX, newY);
}

// Is this type solid?
bool Tile::isSolid() const {
  return tile_pointer.getAttribute();
}

// Modify ID
unsigned char Tile::getID() const {
  return tile_pointer.getID();
}

// Get name of tile
std::string Tile::getName() const {
  return tile_pointer.getName();
}

// Access and set meta data byte
void Tile::setMeta(unsigned char meta) {
  this->meta = meta;
}

unsigned char Tile::getMeta() const {
  return this->meta;
}

void Tile::changeMeta(unsigned char amt) {
  this->meta += amt;
}

// Get image type
bool Tile::needsBitmask() const {
  return this->tile_pointer.getImageType() == "dynamic";
}
