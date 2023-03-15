#include "UiElement.h"

asw::Font UiElement::font{nullptr};

UiElement::UiElement(Vec2<int> pos) : UiElement(pos, Vec2<int>(0, 0)) {}

UiElement::UiElement(Vec2<int> pos, Vec2<int> size) : pos(pos), size(size) {
  if (font == nullptr) {
    font = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
  }
}

Vec2<int> UiElement::getPosition() const {
  return pos;
}

Vec2<int> UiElement::getSize() const {
  return size;
}
