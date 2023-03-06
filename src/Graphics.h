#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <memory>
#include <vector>

#include "Sprite.h"

class Graphics {
 public:
  // Get singleton instance
  static std::shared_ptr<Graphics> Instance();

  // Add and remove sprites
  void add(std::shared_ptr<Sprite> sprite);
  void remove(std::shared_ptr<Sprite> sprite);

  // Disable or enable sorting
  void disableSort();
  void enableSort();

  // Draw managed sprites
  void draw(int x_1, int y_1, int x_2, int y_2) const;

 private:
  // Should sort on add
  bool should_sort = false;

  // Needs a sort
  bool need_sort = false;

  // Sort
  void sort();

  // Drawable
  std::vector<std::shared_ptr<Sprite>> sprites;

  // Single instance
  static std::shared_ptr<Graphics> instance;
};

#endif  // SRC_GRAPHICS_H_
