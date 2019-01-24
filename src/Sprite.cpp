#include "Sprite.h"

#include <iostream>

int Sprite::next_id = 0;

Sprite::Sprite() :
  Sprite(0, 0, 0) {

}

Sprite::Sprite(int x, int y, int z) {
  this -> z = z;
  this -> x = x;
  this -> y = y;

  setId();
}

Sprite::~Sprite() {
  //dtor
}

void Sprite::setZ(int z) {
  this -> z = z;
}


void Sprite::setId() {
  this -> id = Sprite::next_id;
  Sprite::next_id ++;
}
