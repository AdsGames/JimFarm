#include "UI_Element.h"

UI_Element::UI_Element(int x, int y) : UI_Element(x, y, 0, 0) {}

UI_Element::UI_Element(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;

  this->width = width;
  this->height = height;
}

UI_Element::~UI_Element() {}

int UI_Element::GetX() {
  return x;
}

int UI_Element::GetY() {
  return y;
}

int UI_Element::GetWidth() {
  return width;
}

int UI_Element::GetHeight() {
  return height;
}
