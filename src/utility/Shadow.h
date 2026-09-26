#ifndef SRC_UTILITY_SHADOW_H_
#define SRC_UTILITY_SHADOW_H_

#include <asw/asw.h>

#include <algorithm>

namespace shadow {
// Soft dark ellipse under a sprite, centre_x and feet_y in screen pixels.
// Draw it just before the sprite so it lands on the ground below it.
inline void draw(float centre_x, float feet_y, float width) {
  static const auto texture = asw::assets::create_radial_gradient(
      32, asw::Color(0, 0, 0, 110), asw::Color(0, 0, 0, 0));

  const float height = std::max(3.0F, width * 0.4F);
  asw::draw::stretch_sprite(
      texture, asw::Quadf(centre_x - width / 2.0F, feet_y - height / 2.0F,
                          width, height));
}
}  // namespace shadow

#endif  // SRC_UTILITY_SHADOW_H_
