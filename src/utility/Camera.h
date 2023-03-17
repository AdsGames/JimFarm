#ifndef SRC_UTILITY_CAMERA_H_
#define SRC_UTILITY_CAMERA_H_

#include "Quad.h"
#include "Vec2.h"

class Camera {
 public:
  Camera() = default;
  explicit Camera(const Quad<int>& bounds, const Quad<int>& outer_bounds);

  void setZoom(float zoom);
  float getZoom() const;

  void pan(Vec2<int> pos_to);

  Vec2<int> getSize() const;

  Vec2<int> getPosition() const;
  Vec2<int> getCenter() const;

  const Quad<int>& getBounds() const;

 private:
  float zoom{1.0f};

  Quad<int> bounds{};
  Quad<int> outer_bounds{};

  int width{0};
  int height{0};
};

#endif  // SRC_UTILITY_CAMERA_H_