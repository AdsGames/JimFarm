#ifndef SRC_UI_UI_ELEMENT_H_
#define SRC_UI_UI_ELEMENT_H_

#include <asw/asw.h>

class UiElement {
 public:
  explicit UiElement(int x, int y);
  explicit UiElement(int x, int y, int width, int height);
  virtual ~UiElement() = default;

  virtual void draw(int parent_x, int parent_y) = 0;

  int getX() const;
  int getY() const;

  int getWidth() const;
  int getHeight() const;

 protected:
  static asw::Font font;

 private:
  int x;
  int y;
  int width;
  int height;
};

#endif  // SRC_UI_UI_ELEMENT_H_
