#include "UI_Label.h"

UI_Label::UI_Label(int x, int y, std::string text)
    : UI_Element(x, y), text(text) {}

void UI_Label::draw(int parent_x, int parent_y) {
  asw::draw::text(font, text, parent_x + getX(), parent_y + getY(),
                  asw::util::makeColor(255, 255, 255));
}
