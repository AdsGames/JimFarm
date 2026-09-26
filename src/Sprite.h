#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <asw/asw.h>
#include <string>

#include "utility/Camera.h"

constexpr unsigned int MAX_SPRITES = 1000000;

class Sprite {
 public:
  Sprite();
  Sprite(const asw::Vec2i& pos, int z);

  virtual ~Sprite() = default;

  virtual void draw(const Camera& camera) const = 0;

  // Get position
  const asw::Vec2i& getPosition() const;
  float getZ() const;
  unsigned int getSpriteId() const;

 protected:
  asw::Vec2i pos;
  float z;

 private:
  const int id = Sprite::next_id;
  static int next_id;
};

#endif  // SRC_SPRITE_H_
