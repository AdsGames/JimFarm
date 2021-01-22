#include "Messenger.h"

Messenger::Messenger(int listSize,
                     bool topDown,
                     int padding,
                     int font_col,
                     int bg_col) {
  this->max_size = listSize;
  this->topDown = topDown;
  this->padding = padding;
  this->font_col = font_col;
  this->bg_col = bg_col;
}

Messenger::~Messenger() {}

void Messenger::setColors(int font_col, int bg_col) {
  this->font_col = font_col;
  this->bg_col = bg_col;
}

void Messenger::push_message(std::string new_message) {
  msgs.push_back(new_message);
  if (numberMessages() > this->max_size)
    msgs.erase(msgs.begin());
}

unsigned int Messenger::numberMessages() {
  return msgs.size();
}

void Messenger::draw(BITMAP* buffer, int x, int y) {
  int offset = 0;
  int numMsg = (signed)numberMessages();
  for (int i = numMsg - 1; i > -1; i--) {
    textprintf_ex(buffer, font, x, y + offset, font_col, bg_col, ">%s",
                  msgs.at(i).c_str());

    if (topDown)
      offset += text_height(font) + padding;
    else
      offset -= text_height(font) + padding;
  }
}
