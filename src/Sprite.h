#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <asw/asw.h>

class Sprite {
 public:
  Sprite();
  Sprite(int x, int y, int z);

  virtual void draw(float x_1, float y_1, float x_2, float y_2) = 0;

  void setZ(int z);

  // Get position
  int getX() { return this->x; }
  int getY() { return this->y; }
  int getZ() { return this->z; }

  virtual bool operator<(const Sprite& other) const {
    if (z != other.z) {
      return this->z < other.z;
    }
    if (y != other.y) {
      return this->y < other.y;
    }
    return this->id < other.id;
  }

  int x;
  int y;
  int z;
  int id;

 private:
  void setId();
  static int next_id;
};

#endif  // SRC_SPRITE_H_
