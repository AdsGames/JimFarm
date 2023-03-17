#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <map>
#include <memory>
#include <set>
#include <string>

#include "Sprite.h"
#include "utility/Camera.h"

struct SpriteCmp {
  bool operator()(const std::weak_ptr<Sprite>& a,
                  const std::weak_ptr<Sprite>& b) const {
    if (a.expired() || b.expired()) {
      return false;
    }

    if (a.lock()->getZ() != b.lock()->getZ()) {
      return a.lock()->getZ() < b.lock()->getZ();
    }
    if (a.lock()->getPosition().y != b.lock()->getPosition().y) {
      return a.lock()->getPosition().y < b.lock()->getPosition().y;
    }
    return a.lock()->getSpriteId() < b.lock()->getSpriteId();
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

  // Prune dead sprites
  void prune();

 private:
  // Drawable
  std::map<unsigned int, std::weak_ptr<Sprite>> sprites{};

  bool needs_sorting = false;

  // Single instance
  static std::shared_ptr<Graphics> instance;
};

#endif  // SRC_GRAPHICS_H_
