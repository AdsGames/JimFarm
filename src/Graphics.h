#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Sprite.h"
#include "utility/Camera.h"

// Sort key is captured on insert so comparisons never touch the sprite
struct SpriteEntry {
  float z;
  int y;
  unsigned int id;
  std::weak_ptr<Sprite> sprite;

  bool operator<(const SpriteEntry& other) const {
    if (z != other.z) {
      return z < other.z;
    }
    if (y != other.y) {
      return y < other.y;
    }
    return id < other.id;
  }
};

class Graphics {
 public:
  // Get singleton instance
  static Graphics& Instance();

  // Add and remove sprites
  void add(const std::shared_ptr<Sprite>& sprite, bool dynamic = false);
  void remove(const std::shared_ptr<Sprite>& sprite);

  // Draw managed sprites
  void draw(const Camera& camera) const;

  // Re-sort dynamic sprites and drop dead ones
  void prune();

 private:
  using SortedSet = std::set<SpriteEntry>;

  void erase(unsigned int id);

  // Drawable
  SortedSet sorted_sprites{};
  std::unordered_map<unsigned int, SortedSet::iterator> sprites{};
  std::unordered_set<unsigned int> dynamic_sprites{};
};

#endif  // SRC_GRAPHICS_H_
