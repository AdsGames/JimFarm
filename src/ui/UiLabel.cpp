#include "UiLabel.h"

UiLabel::UiLabel(const asw::Vec2i& pos, const std::string& text)
    : UiElement(pos), text(text) {}

void UiLabel::draw(const asw::Vec2i& parent_pos) {
  auto draw_pos = parent_pos + getPosition();

  asw::draw::text(font, text, asw::Vec2f(draw_pos.x, draw_pos.y),
                  asw::Color(255, 255, 255));
}
