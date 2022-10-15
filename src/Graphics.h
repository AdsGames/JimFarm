#ifndef SRC_GRAPHICS_H_
#define SRC_GRAPHICS_H_

#include <vector>

#include "Sprite.h"

class Graphics {
 public:
  // Get singleton instance
  static Graphics* Instance();

  // Add and remove sprites
  void add(Sprite* sprite);
  void remove(Sprite* sprite);

  // Disable or enable sorting
  void disableSort();
  void enableSort();

  // Draw managed sprites
  void draw(int x_1, int y_1, int x_2, int y_2);

 private:
  // Prevent instantiation
  Graphics();

  // Should sort on add
  bool should_sort;

  // Needs a sort
  bool need_sort;

  // Sort
  void sort();

  // Drawable
  std::vector<Sprite*> sprites;

  // Single instance
  static Graphics* instance;
};

#endif  // SRC_GRAPHICS_H_
