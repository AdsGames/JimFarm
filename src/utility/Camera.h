#ifndef SRC_UTILITY_CAMERA_H_
#define SRC_UTILITY_CAMERA_H_

#include <asw/asw.h>

#include "Quad.h"

class Camera {
 public:
  Camera() = default;
  explicit Camera(const Quad<int>& bounds, const Quad<int>& outer_bounds);

  void setZoom(float zoom);
  float getZoom() const;

  void pan(const asw::Vec2i& pos_to);

  const asw::Vec2i getSize() const;
  const asw::Vec2i getPosition() const;
  const asw::Vec2i getCenter() const;
  const Quad<int>& getBounds() const;

 private:
  float zoom{1.0f};

  Quad<int> bounds{};
  Quad<int> outer_bounds{};

  int width{0};
  int height{0};
};

#endif  // SRC_UTILITY_CAMERA_H_