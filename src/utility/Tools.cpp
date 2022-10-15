#include "Tools.h"

#include <time.h>

// Random number generator. Use int random(highest,lowest);
int random(int min, int max) {
  return (rand() % (max + 1 - min)) + min;
}

// Collision
bool collision(int xMin1,
               int xMax1,
               int xMin2,
               int xMax2,
               int yMin1,
               int yMax1,
               int yMin2,
               int yMax2) {
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1) {
    return true;
  }
  return false;
}
