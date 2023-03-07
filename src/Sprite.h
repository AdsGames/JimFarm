#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <asw/asw.h>

class Sprite {
 public:
  Sprite();
  Sprite(int x, int y, int z);
  virtual ~Sprite() = default;

  virtual void draw(int x_1, int y_1, int x_2, int y_2) const = 0;

  void setZ(int z);

  // Get position
  int getX() const { return this->x; }
  int getY() const { return this->y; }
  int getZ() const { return this->z; }

  bool operator<(const Sprite& other) const {
    if (z != other.z) {
      return this->z < other.z;
    }
    if (y != other.y) {
      return this->y < other.y;
    }
    return this->id < other.id;
  }

 protected:
  int x;
  int y;
  int z;
  int id;

 private:
  void setId();
  static int next_id;
};

#endif  // SRC_SPRITE_H_
