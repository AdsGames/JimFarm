#ifndef SRC_UTILITY_QUAD_H_
#define SRC_UTILITY_QUAD_H_

template <typename T>
class Quad {
 public:
  bool contains(T x, T y) const {
    return x >= x_1 && x <= x_2 && y >= y_1 && y <= y_2;
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