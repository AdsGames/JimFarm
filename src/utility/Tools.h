#ifndef SRC_UTILITY_TOOLS_H_
#define SRC_UTILITY_TOOLS_H_

#include <random>
#include <string>

extern std::default_random_engine generator;

// Random number generator. Use int random(highest,lowest);
template <class T>
T random(T min, T max) {
  std::uniform_int_distribution<T> distribution(min, max);
  return distribution(generator);
}

extern bool collision(int xMin1,
                      int xMax1,
                      int xMin2,
                      int xMax2,
                      int yMin1,
                      int yMax1,
                      int yMin2,
                      int yMax2);

// Clamp values
template <class T>
static T clamp(T min_val, T max_val, T value) {
  if (value < min_val)
    return min_val;
  if (value > max_val)
    return max_val;
  return value;
}

// Clamp values
template <class T>
static T negative_clamp(T min_val, T max_val, T value) {
  if (value < min_val)
    return min_val + -1 * value;
  if (value > max_val)
    return max_val - value;
  return value;
}

#endif  // SRC_UTILITY_TOOLS_H_
