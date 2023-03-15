#include "Tile.h"

#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

// Ctor for tile
Tile::Tile(const std::string& id, Vec2<int> pos, int z, unsigned char meta)
    : Sprite(pos, z), meta(meta), tile_pointer(TileTypeManager::getTile(id)) {}

Vec2<int> Tile::getTilePosition() const {
  return pos / TILE_SIZE;
}

// Draw tile to screen
void Tile::draw(const Camera& camera) const {
  tile_pointer.draw(pos.x - camera.getPosition().x,
                    pos.y - camera.getPosition().y, getMeta());
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
