#include "Sprite.h"

int Sprite::next_id = 0;

Sprite::Sprite() {
  Sprite::next_id++;
}

Sprite::Sprite(const asw::Vec2i& pos, int z) : pos(pos), z(z) {
  Sprite::next_id++;
}

const asw::Vec2i& Sprite::getPosition() const {
  return this->pos;
}

float Sprite::getZ() const {
  return this->z;
}

unsigned int Sprite::getSpriteId() const {
  return this->id;
}