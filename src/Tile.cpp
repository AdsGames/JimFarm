#include "Tile.h"

#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

// Ctor for tile
Tile::Tile(const std::string& id, int x, int y, int z, unsigned char meta)
    : Sprite(x, y, z), meta(meta), tile_pointer(TileTypeManager::getTile(id)) {}

int Tile::getTileX() const {
  return x / TILE_SIZE;
}

int Tile::getTileY() const {
  return y / TILE_SIZE;
}

// Draw tile to screen
void Tile::draw(const Camera& camera) const {
  tile_pointer.draw(x - camera.getX(), y - camera.getY(), getMeta());
}

const TileType& Tile::getType() const {
  return tile_pointer;
}

// Access and set meta data byte
void Tile::setMeta(unsigned char new_meta) {
  this->meta = new_meta;
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
