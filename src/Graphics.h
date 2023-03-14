#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <map>
#include <memory>
#include <set>
#include <string>

#include "Sprite.h"
#include "utility/Camera.h"

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
  void remove(std::shared_ptr<Sprite> sprite);

  // Draw managed sprites
  void draw(const Camera& camera) const;

 private:
  // Drawable
  std::map<unsigned int, std::shared_ptr<Sprite>> sprites{};

  bool needs_sorting = false;

  // Single instance
  static std::shared_ptr<Graphics> instance;
};

#endif  // SRC_GRAPHICS_H_
