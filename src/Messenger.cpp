#include "Messenger.h"

Messenger::Messenger(unsigned int list_size, bool is_top_down, int padding)
    : max_size(list_size), top_down(is_top_down), padding(padding) {
  this->pixelart = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
}

void Messenger::setColors(asw::Color font, asw::Color background) {
  this->font_color = font;
  this->bg_color = background;
}

void Messenger::pushMessage(const std::string& message) {
  msgs.push_back(message);
  if (messageCount() > this->max_size) {
    msgs.erase(msgs.begin());
  }
}

size_t Messenger::messageCount() const {
  return msgs.size();
}

void Messenger::draw(int x, int y) {
  int offset = 0;
  auto num_messages = messageCount();
  auto font_size = asw::util::getTextSize(pixelart, " ");

  for (auto i = num_messages - 1; i > -1; i--) {
    asw::draw::text(pixelart, ">" + msgs.at(i), x, y + offset, font_color);

    if (top_down) {
      offset += font_size.y + padding;
    } else {
      offset -= font_size.y + padding;
    }
  }
}
