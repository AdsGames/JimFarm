#ifndef SRC_UTILITY_TOOLS_H_
#define SRC_UTILITY_TOOLS_H_

#define MAIN_MENU 0
#define HELP 1
#define OPTIONS 2
#define STORY 3

#include <allegro.h>
#include <string>

// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const std::string& message);

// Load and error check images
extern BITMAP* loadBitmap(const std::string& path);

// Load and error check sounds
extern SAMPLE* loadSample(const std::string& path);

// Load and error check fonts
extern FONT* loadFont(const std::string& path);

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
