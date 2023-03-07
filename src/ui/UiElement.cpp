#include "UiElement.h"

asw::Font UiElement::font{nullptr};

UiElement::UiElement(int x, int y) : UiElement(x, y, 0, 0) {}

UiElement::UiElement(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height) {
  if (font == nullptr) {
    font = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
  }
}

int UiElement::getX() const {
  return x;
}

int UiElement::getY() const {
  return y;
}

int UiElement::getWidth() const {
  return width;
}

int UiElement::getHeight() const {
  return height;
}
