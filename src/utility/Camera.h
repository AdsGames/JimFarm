#ifndef SRC_UTILITY_CAMERA_H_
#define SRC_UTILITY_CAMERA_H_

#include "Quad.h"

class Camera {
 public:
  Camera() = default;
  explicit Camera(const Quad<int>& bounds, const Quad<int>& outer_bounds);

  void setZoom(float zoom);
  float getZoom() const;

  void translate(int x, int y);
  void pan(int x, int y);

  int getWidth() const;
  int getHeight() const;

  int getX() const;
  int getY() const;

  const Quad<int>& getBounds() const;

 private:
  float zoom{1.0f};

  Quad<int> bounds{};
  Quad<int> outer_bounds{};

  int width{0};
  int height{0};
};

#endif  // SRC_UTILITY_CAMERA_H_