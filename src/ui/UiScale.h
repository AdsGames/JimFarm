#ifndef SRC_UI_UI_SCALE_H_
#define SRC_UI_UI_SCALE_H_

#include <asw/asw.h>

// UI is laid out in UI units, rendered to a buffer and stretched by this factor
constexpr int UI_SCALE = 2;

// Screen size in UI units
inline asw::Vec2i getUiSize() {
  const auto size = asw::display::get_logical_size();
  return asw::Vec2i(size.x / UI_SCALE, size.y / UI_SCALE);
}

// Mouse position in UI units
inline asw::Vec2i getUiMouse() {
  const auto& position = asw::input::get_mouse().position;
  return asw::Vec2i(static_cast<int>(position.x) / UI_SCALE,
                    static_cast<int>(position.y) / UI_SCALE);
}

#endif  // SRC_UI_UI_SCALE_H_
