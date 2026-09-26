#include "UiElement.h"

#include "../utility/Fonts.h"

asw::Font UiElement::font{nullptr};

UiElement::UiElement(const asw::Vec2i& pos)
    : UiElement(pos, asw::Vec2i(0, 0)) {}

UiElement::UiElement(const asw::Vec2i& pos, const asw::Vec2i& size)
    : pos(pos), size(size) {
  if (font == nullptr) {
    font = fonts::load();
  }
}

const asw::Vec2i& UiElement::getPosition() const {
  return pos;
}

const asw::Vec2i& UiElement::getSize() const {
  return size;
}
