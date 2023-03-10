#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <memory>
#include <set>

#include "Sprite.h"

struct SpriteCmp {
  bool operator()(const std::shared_ptr<Sprite>& a,
                  const std::shared_ptr<Sprite>& b) const {
    if (a->getZ() != b->getZ()) {
      return a->getZ() < b->getZ();
    }
    if (a->getY() != b->getY()) {
      return a->getY() < b->getY();
    }
    return a->getSpriteId() < b->getSpriteId();
  }
};

class Graphics {
 public:
  // Get singleton instance
  static std::shared_ptr<Graphics> Instance();

  // Add and remove sprites
  void add(std::shared_ptr<Sprite> sprite);
  void remove(const unsigned int sprite_id);

  // Draw managed sprites
  void draw(int x_1, int y_1, int x_2, int y_2) const;

 private:
  // Drawable
  std::set<std::shared_ptr<Sprite>, SpriteCmp> sprites;

  // Single instance
  static std::shared_ptr<Graphics> instance;
};

#endif  // SRC_GRAPHICS_H_
