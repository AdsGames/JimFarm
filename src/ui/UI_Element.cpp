#include "UI_Element.h"

asw::Font UI_Element::font{nullptr};

UI_Element::UI_Element(int x, int y) : UI_Element(x, y, 0, 0) {}

UI_Element::UI_Element(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height) {
  if (font == nullptr) {
    font = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
  }
}

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
