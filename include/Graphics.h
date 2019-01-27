#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

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
    void draw(BITMAP *buffer, int x_1, int y_1, int x_2, int y_2);
  private:
    // Prevent instantiation
    Graphics();

    // Should sort on add
    bool should_sort;

    // Sort
    void sort();

    // Drawable
    std::vector<Sprite*> sprites;

    // Single instance
    static Graphics *instance;
};

#endif // SPRITEDRAW_H
