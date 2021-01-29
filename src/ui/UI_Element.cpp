#include "UI_Element.h"

UI_Element::UI_Element(int x, int y) : UI_Element(x, y, 0, 0) {}

UI_Element::UI_Element(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;

  this->width = width;
  this->height = height;
}

UI_Element::~UI_Element() {}

int UI_Element::getX() {
  return x;
}

int UI_Element::getY() {
  return y;
}

int UI_Element::getWidth() {
  return width;
}

int UI_Element::getHeight() {
  return height;
}
