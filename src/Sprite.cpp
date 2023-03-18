#include "Sprite.h"

int Sprite::next_id = 0;

Sprite::Sprite() {
  Sprite::next_id++;
}

Sprite::Sprite(Vec2<int> pos, int z) : pos(pos), z(z) {
  Sprite::next_id++;
}

Vec2<int> Sprite::getPosition() const {
  return this->pos;
}

float Sprite::getZ() const {
  return this->z;
}

unsigned int Sprite::getSpriteId() const {
  return this->id;
}