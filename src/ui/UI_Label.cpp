#include "UI_Label.h"

UI_Label::UI_Label(int x, int y, std::string text) : UI_Element(x, y) {
  this->text = text;
}

UI_Label::~UI_Label() {}

void UI_Label::draw(BITMAP* buffer, int parent_x, int parent_y) {
  textprintf_ex(buffer, font, parent_x + getX(), parent_y + getY(),
                makecol(255, 255, 255), -1, "%s", this->text.c_str());
}
