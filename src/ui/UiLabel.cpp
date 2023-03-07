#include "UiLabel.h"

UiLabel::UiLabel(int x, int y, const std::string& text)
    : UiElement(x, y), text(text) {}

void UiLabel::draw(int parent_x, int parent_y) {
  asw::draw::text(font, text, parent_x + getX(), parent_y + getY(),
                  asw::util::makeColor(255, 255, 255));
}
