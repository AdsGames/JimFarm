#ifndef SRC_UI_UI_ELEMENT_H_
#define SRC_UI_UI_ELEMENT_H_

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

#endif  // SRC_UI_UI_ELEMENT_H_
