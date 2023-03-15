#include "UiLabel.h"

UiLabel::UiLabel(Vec2<int> pos, const std::string& text)
    : UiElement(pos), text(text) {}

void UiLabel::draw(Vec2<int> parent_pos) {
  auto draw_pos = parent_pos + getPosition();

  asw::draw::text(font, text, draw_pos.x, draw_pos.y,
                  asw::util::makeColor(255, 255, 255));
}
