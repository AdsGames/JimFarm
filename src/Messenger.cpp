#include "Messenger.h"

Messenger::Messenger(int listSize,
                     bool topDown,
                     int padding,
                     asw::Color font_col,
                     asw::Color bg_col)
    : max_size(listSize),
      topDown(topDown),
      padding(padding),
      font_col(font_col),
      bg_col(bg_col) {
  pixelart = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
}

void Messenger::setColors(asw::Color font_col, asw::Color bg_col) {
  this->font_col = font_col;
  this->bg_col = bg_col;
}

void Messenger::pushMessage(std::string new_message) {
  msgs.push_back(new_message);
  if (messageCount() > this->max_size)
    msgs.erase(msgs.begin());
}

unsigned int Messenger::messageCount() {
  return msgs.size();
}

void Messenger::draw(int x, int y) {
  int offset = 0;
  int numMsg = (signed)messageCount();
  for (int i = numMsg - 1; i > -1; i--) {
    asw::draw::text(pixelart, ">" + msgs.at(i), x, y + offset, font_col);

    auto fontSize = asw::util::getTextSize(pixelart, " ");

    if (topDown) {
      offset += fontSize.y + padding;
    } else {
      offset -= fontSize.y + padding;
    }
  }
}
