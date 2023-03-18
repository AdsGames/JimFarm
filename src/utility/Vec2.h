#pragma once

template <class T>
class Vec2 {
 public:
  /// @brief The x component
  T x;

  /// @brief The y component
  T y;

  /**
   * @brief Construct a new Vec2 object
   * @details Creates a new Vec2 object with the given x and y components
   *
   * @param x - The x component
   * @param y - The y component
   */
  Vec2(T x, T y) : x(x), y(y) {}

  /**
   * @brief Construct a new Vec 2 object
   * @details Creates a new Vec2 object with the x and y components set to 0
   *
   */
  Vec2() : x(0), y(0) {}

  /**
   * @brief Addition assignment operator
   *
   * @param other The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  /**
   * @brief Subtraction assignment operator
   *
   * @param other The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  /**
   * @brief Multiplication assignment operator
   *
   * @param other The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator*=(const Vec2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
  }

  /**
   * @brief Division assignment operator
   *
   * @param other The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator/=(const Vec2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
  }

  /**
   * @brief Addition operator
   *
   * @param other The other Vec2 object
   * @return Vec2
   */
  Vec2 operator+(const Vec2& other) const {
    return Vec2(x + other.x, y + other.y);
  }

  /**
   * @brief Subtraction operator
   *
   * @param other The other Vec2 object
   * @return Vec2
   */
  Vec2 operator-(const Vec2& other) const {
    return Vec2(x - other.x, y - other.y);
  }

  /**
   * @brief Multiplication operator
   *
   * @param other The other Vec2 object
   * @return Vec2
   */
  Vec2 operator*(const Vec2& other) const {
    return Vec2(x * other.x, y * other.y);
  }

  /**
   * @brief Division operator
   *
   * @param other The other Vec2 object
   * @return Vec2
   */
  Vec2 operator/(const Vec2& other) const {
    return Vec2(x / other.x, y / other.y);
  }

  /**
   * @brief Equality operator
   *
   * @param other The other Vec2 object
   * @return Vec2&
   */
  bool operator==(const Vec2& other) const {
    return x == other.x && y == other.y;
  }

  /**
   * @brief Add a scalar to the Vec2 object
   *
   * @param scalar The scalar to add
   * @return Vec2&
   */
  Vec2 operator+(const T scalar) const { return Vec2(x + scalar, y + scalar); }

  /**
   * @brief Subtract a scalar from the Vec2 object
   *
   * @param scalar The scalar to subtract
   * @return Vec2
   */
  Vec2 operator-(const T scalar) const { return Vec2(x - scalar, y - scalar); }

  /**
   * @brief Multiply the Vec2 object by a scalar
   *
   * @param scalar The scalar to multiply by
   * @return Vec2
   */
  Vec2 operator*(const T scalar) const { return Vec2(x * scalar, y * scalar); }

  /**
   * @brief Divide the Vec2 object by a scalar
   *
   * @param scalar The scalar to divide by
   * @return Vec2
   */
  Vec2 operator/(const T scalar) const { return Vec2(x / scalar, y / scalar); }

  /**
   * @brief Modulo the Vec2 object by a scalar
   *
   * @param scalar The scalar to modulo by
   * @return Vec2
   */
  Vec2 operator%(const T scalar) const { return Vec2(x % scalar, y % scalar); }
};