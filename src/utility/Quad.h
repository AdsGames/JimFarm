#ifndef SRC_UTILITY_QUAD_H_
#define SRC_UTILITY_QUAD_H_

#include "./Vec2.h"

template <typename T>
class Quad {
 public:
  bool contains(const Vec2<T>& position) const {
    return position.x >= x_1 && position.x <= x_2 && position.y >= y_1 &&
           position.y <= y_2;
  }

  bool intersects(const Quad& other) const {
    return !(other.x_2 < x_1 || other.x_1 > x_2 || other.y_2 < y_1 ||
             other.y_1 > y_2);
  }

  T getWidth() const { return x_2 - x_1; }

  T getHeight() const { return y_2 - y_1; }

  T x_1{0};
  T y_1{0};
  T x_2{0};
  T y_2{0};
};

#endif  // SRC_UTILITY_QUAD_H_