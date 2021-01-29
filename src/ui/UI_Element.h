#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <allegro.h>

class UI_Element {
 public:
  explicit UI_Element(int x, int y);
  explicit UI_Element(int x, int y, int width, int height);
  virtual ~UI_Element();

  virtual void draw(BITMAP* buffer, int parent_x, int parent_y) = 0;

  int getX();
  int getY();

  int getWidth();
  int getHeight();

 protected:
 private:
  int x, y;
  int width, height;
};

#endif  // UI_ELEMENT_H
