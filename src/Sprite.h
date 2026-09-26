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

  // World pixel area the sprite may draw into, used to cull off screen
  // sprites. The default is generous for characters, creatures and items.
  virtual Quad<int> getDrawBounds() const;

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
