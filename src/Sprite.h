#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <asw/asw.h>
#include <string>

#include "utility/Camera.h"

constexpr unsigned int MAX_SPRITES = 1000000;

class Sprite {
 public:
  Sprite();
  Sprite(int x, int y, int z);
  virtual ~Sprite() = default;

  virtual void draw(const Camera& camera) const = 0;

  // Get position
  int getX() const { return this->x; }
  int getY() const { return this->y; }
  int getZ() const { return this->z; }

  unsigned int getSpriteId() const { return this->id; }

 protected:
  int x;
  int y;
  int z;

 private:
  const int id = Sprite::next_id;
  static int next_id;
};

#endif  // SRC_SPRITE_H_
