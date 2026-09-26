#include "ActionParticles.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>

#include "utility/Tools.h"

namespace {
constexpr std::size_t MAX_PARTICLES = 400;
constexpr float PI = 3.14159265F;

struct Preset {
  std::vector<asw::Color> colors;
  int count;

  // Launch speed in px/s and angle spread around straight up, in radians
  float speed_min;
  float speed_max;
  float spread;

  float gravity;
  float life;
  float size_max;
};

const std::map<std::string, Preset, std::less<>> PRESETS = {
    {"dirt",
     {{asw::Color(120, 84, 52), asw::Color(96, 66, 40), asw::Color(150, 110, 70)},
      10, 30.0F, 70.0F, 1.2F, 220.0F, 0.45F, 2.0F}},
    {"water",
     {{asw::Color(120, 170, 255, 220), asw::Color(180, 210, 255, 220)},
      10, 25.0F, 60.0F, 1.0F, 240.0F, 0.4F, 2.0F}},
    {"grass",
     {{asw::Color(90, 170, 60), asw::Color(130, 200, 80), asw::Color(70, 140, 50)},
      8, 25.0F, 55.0F, 1.4F, 90.0F, 0.6F, 2.0F}},
    {"wood",
     {{asw::Color(170, 120, 70), asw::Color(210, 170, 110), asw::Color(120, 80, 45)},
      9, 40.0F, 80.0F, 1.1F, 260.0F, 0.5F, 2.0F}},
    {"leaves",
     {{asw::Color(60, 140, 60), asw::Color(90, 170, 70), asw::Color(200, 150, 60)},
      10, 20.0F, 50.0F, 1.6F, 60.0F, 0.9F, 2.0F}},
    {"feathers",
     {{asw::Color(245, 245, 240), asw::Color(225, 220, 210)},
      6, 15.0F, 35.0F, 1.6F, 40.0F, 0.9F, 2.0F}},
    {"stone",
     {{asw::Color(150, 150, 155), asw::Color(110, 110, 118), asw::Color(185, 180, 175)},
      9, 40.0F, 85.0F, 1.1F, 280.0F, 0.45F, 2.0F}},
    {"smoke",
     {{asw::Color(170, 170, 170, 170), asw::Color(130, 130, 135, 150)},
      7, 8.0F, 20.0F, 0.5F, -15.0F, 1.2F, 3.0F}},
    {"sparks",
     {{asw::Color(255, 200, 80), asw::Color(255, 140, 40), asw::Color(255, 240, 160)},
      10, 25.0F, 60.0F, 0.6F, -20.0F, 0.6F, 1.0F}},
    // Single particles, emitted steadily by lit campfires
    {"fire_smoke",
     {{asw::Color(160, 160, 165, 130), asw::Color(120, 120, 125, 110)},
      1, 10.0F, 18.0F, 0.35F, -6.0F, 1.8F, 3.0F}},
    {"fire_spark",
     {{asw::Color(255, 200, 80), asw::Color(255, 150, 40), asw::Color(255, 240, 170)},
      1, 20.0F, 45.0F, 0.5F, -10.0F, 0.7F, 1.0F}},
    {"coins",
     {{asw::Color(255, 215, 70), asw::Color(255, 240, 150), asw::Color(220, 170, 40)},
      12, 50.0F, 95.0F, 0.8F, 230.0F, 0.7F, 2.0F}},
};

float randomFloat(float min, float max) {
  return min + (max - min) * static_cast<float>(random(0, 1000)) / 1000.0F;
}
}  // namespace

void ActionParticles::burst(const std::string& preset, const asw::Vec2f& pos) {
  const auto found = PRESETS.find(preset);
  if (found == PRESETS.end()) {
    return;
  }

  const auto& p = found->second;
  for (int i = 0; i < p.count; i++) {
    if (particles.size() >= MAX_PARTICLES) {
      particles.erase(particles.begin());
    }

    // Upward cone, 0 is straight up
    const float angle = randomFloat(-p.spread, p.spread) - PI / 2.0F;
    const float speed = randomFloat(p.speed_min, p.speed_max);
    const auto& color = p.colors[random<size_t>(0, p.colors.size() - 1)];

    particles.push_back(
        {asw::Vec2f(pos.x + randomFloat(-3.0F, 3.0F),
                    pos.y + randomFloat(-2.0F, 2.0F)),
         asw::Vec2f(std::cos(angle) * speed, std::sin(angle) * speed), color,
         p.gravity, std::round(randomFloat(1.0F, p.size_max)), 0.0F,
         p.life * randomFloat(0.7F, 1.2F)});
  }
}

void ActionParticles::update(float dt) {
  for (auto& p : particles) {
    p.age += dt;
    p.vel.y += p.gravity * dt;
    p.vel.x *= 1.0F - std::min(1.0F, 2.0F * dt);
    p.pos += p.vel * dt;
  }

  std::erase_if(particles, [](const Particle& p) { return p.age >= p.life; });
}

void ActionParticles::draw(const Camera& camera) const {
  const auto offset = camera.getPosition();

  for (auto const& p : particles) {
    // Solid at first, fade over the last half of the life
    const float fade = std::clamp(2.0F - 2.0F * p.age / p.life, 0.0F, 1.0F);
    const auto alpha = static_cast<uint8_t>(p.color.a * fade);

    asw::draw::rect_fill(
        asw::Quadf(std::round(p.pos.x - static_cast<float>(offset.x)),
                   std::round(p.pos.y - static_cast<float>(offset.y)), p.size,
                   p.size),
        p.color.with_alpha(alpha));
  }
}
