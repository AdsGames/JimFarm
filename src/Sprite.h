#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <asw/asw.h>
#include <string>

#include "utility/Camera.h"
#include "utility/Vec2.h"

constexpr unsigned int MAX_SPRITES = 1000000;

class Sprite {
 public:
  Sprite();
  Sprite(Vec2<int> pos, int z);

  virtual ~Sprite() = default;

  virtual void draw(const Camera& camera) const = 0;

  // Get position
  Vec2<int> getPosition() const;
  float getZ() const;
  unsigned int getSpriteId() const;

 protected:
  Vec2<int> pos;
  float z;

 private:
  const int id = Sprite::next_id;
  static int next_id;
};

#endif  // SRC_SPRITE_H_
