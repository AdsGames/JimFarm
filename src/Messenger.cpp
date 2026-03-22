#include "Messenger.h"

#include <algorithm>

namespace {
// Seconds a message stays up, the last second fades out
constexpr float MESSAGE_LIFETIME = 6.0F;
constexpr float MESSAGE_FADE = 1.0F;
}  // namespace

Messenger::Messenger(unsigned int list_size, bool is_top_down, int padding)
    : max_size(list_size), top_down(is_top_down), padding(padding) {
  this->pixelart = asw::assets::load_font("assets/fonts/pixelart.ttf", 8,
                                          asw::FontStyle::Pixel);
}

void Messenger::setColors(asw::Color font, asw::Color background) {
  this->font_color = font;
  this->bg_color = background;
}

void Messenger::pushMessage(const std::string& message) {
  msgs.push_back({message, 0.0F});
  if (messageCount() > this->max_size) {
    msgs.erase(msgs.begin());
  }
}

size_t Messenger::messageCount() const {
  return msgs.size();
}

void Messenger::update(float dt) {
  for (auto& msg : msgs) {
    msg.age += dt;
  }

  std::erase_if(msgs, [](const Message& msg) {
    return msg.age >= MESSAGE_LIFETIME;
  });
}

void Messenger::draw(int x, int y) const {
  int offset = 0;
  auto font_size = asw::util::get_text_size(pixelart, " ");

  for (const auto& msg : msgs) {
    // Fade in 16 steps, each alpha is a separate cached text texture
    const float left = MESSAGE_LIFETIME - msg.age;
    const float fade = std::clamp(left / MESSAGE_FADE, 0.0F, 1.0F);
    auto color = font_color;
    color.a = static_cast<uint8_t>(static_cast<int>(fade * 15.0F) * 17);

    asw::draw::text(pixelart, ">" + msg.text, asw::Vec2f(x, y + offset),
                    color);

    if (top_down) {
      offset += font_size.y + padding;
    } else {
      offset -= font_size.y + padding;
    }
  }
}
