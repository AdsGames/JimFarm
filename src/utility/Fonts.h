#ifndef SRC_UTILITY_FONTS_H_
#define SRC_UTILITY_FONTS_H_

#include <asw/asw.h>

namespace fonts {
// Tiny5 is drawn on an 8 px grid, it is only sharp at whole multiples of it
constexpr int GRID_SIZE = 8;

// Game font at scale times its grid size, e.g. 2 for 16 px menu text
inline asw::Font load(int scale = 1) {
  return asw::assets::load_font("assets/fonts/Tiny5-Regular.ttf",
                                static_cast<float>(GRID_SIZE * scale),
                                asw::FontStyle::Pixel);
}
}  // namespace fonts

#endif  // SRC_UTILITY_FONTS_H_
