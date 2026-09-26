#ifndef SRC_UTILITY_ANIM_H_
#define SRC_UTILITY_ANIM_H_

#include <asw/asw.h>

#include <cmath>

// Small cosmetic animation helpers shared by creatures and animal tiles
namespace anim {

// Seconds on a loop, only used for looping animation
inline float time() {
  return static_cast<float>(SDL_GetTicks() % 3600000) / 1000.0F;
}

// Per entity offset so animals do not bob in step
inline float phase(unsigned int seed) {
  return static_cast<float>((seed * 2654435761U) % 628U) / 100.0F;
}

// Slow breathing bob, 0 to -amplitude pixels
inline int idleBob(unsigned int seed, float amplitude = 1.0F) {
  const float wave = (std::sin(time() * 3.0F + phase(seed)) + 1.0F) * 0.5F;
  return -static_cast<int>(std::lround(wave * amplitude));
}

// Fast hop while walking, 0 to -amplitude pixels
inline int walkBob(unsigned int seed, float amplitude = 2.0F) {
  const float wave = std::abs(std::sin(time() * 12.0F + phase(seed)));
  return -static_cast<int>(std::lround(wave * amplitude));
}

}  // namespace anim

#endif  // SRC_UTILITY_ANIM_H_
