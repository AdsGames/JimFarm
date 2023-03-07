#include "Sprite.h"

int Sprite::next_id = 0;

Sprite::Sprite() : Sprite(0, 0, 0) {}

Sprite::Sprite(int x, int y, int z) : x(x), y(y), z(z) {
  this->setId();
}

void Sprite::setZ(int new_z) {
  this->z = new_z;
}

void Sprite::setId() {
  this->id = Sprite::next_id;
  Sprite::next_id++;
}
