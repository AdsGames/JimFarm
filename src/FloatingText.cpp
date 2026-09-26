#include "FloatingText.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float LIFETIME = 0.8F;
constexpr float RISE_SPEED = 18.0F;
constexpr std::size_t MAX_TEXTS = 32;

// Few alpha steps so the text cache is not flooded while fading
constexpr float ALPHA_STEPS = 8.0F;
}  // namespace

void FloatingTexts::add(const std::string& text,
                        const asw::Vec2f& pos,
                        const asw::Color& color) {
  if (texts.size() >= MAX_TEXTS) {
    texts.erase(texts.begin());
  }

  texts.push_back({text, pos, color, 0.0F});
}

void FloatingTexts::update(float dt) {
  for (auto& entry : texts) {
    entry.age += dt;
    entry.pos.y -= RISE_SPEED * dt * (1.0F - entry.age / LIFETIME);
  }

  std::erase_if(texts,
                [](const Entry& entry) { return entry.age >= LIFETIME; });
}

void FloatingTexts::draw(const asw::Font& font, const Camera& camera) const {
  const auto offset = camera.getPosition();

  for (auto const& entry : texts) {
    // Solid for the first half, then fade out
    const float fade =
        std::clamp(2.0F - 2.0F * entry.age / LIFETIME, 0.0F, 1.0F);
    const float stepped = std::ceil(fade * ALPHA_STEPS) / ALPHA_STEPS;
    const auto alpha = static_cast<uint8_t>(stepped * 255.0F);

    const auto at = asw::Vec2f(entry.pos.x - static_cast<float>(offset.x),
                               entry.pos.y - static_cast<float>(offset.y));

    asw::draw::text(font, entry.text, asw::Vec2f(at.x + 1, at.y + 1),
                    asw::Color(0, 0, 0, alpha), asw::TextJustify::Center);
    asw::draw::text(font, entry.text, at, entry.color.with_alpha(alpha),
                    asw::TextJustify::Center);
  }
}
