#ifndef SRC_UI_UI_ELEMENT_H_
#define SRC_UI_UI_ELEMENT_H_

#include <asw/asw.h>

class UI_Element {
 public:
  explicit UI_Element(int x, int y);
  explicit UI_Element(int x, int y, int width, int height);
  virtual ~UI_Element() = default;

  virtual void draw(int parent_x, int parent_y) = 0;

  int getX();
  int getY();

  int getWidth();
  int getHeight();

 protected:
  static asw::Font font;

 private:
  int x;
  int y;
  int width;
  int height;
};

#endif  // SRC_UI_UI_ELEMENT_H_
