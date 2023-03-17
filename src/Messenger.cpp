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

void Messenger::draw(int x, int y) const {
  int offset = 0;
  auto font_size = asw::util::getTextSize(pixelart, " ");

  for (const auto& i : msgs) {
    asw::draw::text(pixelart, ">" + i, x, y + offset, font_color);

    if (top_down) {
      offset += font_size.y + padding;
    } else {
      offset -= font_size.y + padding;
    }
  }
}
