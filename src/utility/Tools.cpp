#include "Tools.h"

#include <loadpng.h>
#include <png.h>
#include <time.h>

// Random number generator. Use int random(highest,lowest);
int random(int min, int max) {
  return (rand() % (max + 1 - min)) + min;
}

// ERROR REPORTING
void abort_on_error(const std::string& message) {
  if (screen != nullptr) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  }
  allegro_message("%s.\n %s\n", message.c_str(), allegro_error);
  exit(-1);
}

// Load bitmap with error checked_array_iterator
BITMAP* loadBitmap(const std::string& path) {
  BITMAP* temp_loader;
  if (!(temp_loader = load_png(path.c_str(), nullptr)))
    abort_on_error(("Cannot find image " + path +
                    "\nPlease check your files and try again")
                       .c_str());

  return temp_loader;
}

// Load and error check sounds
SAMPLE* loadSample(const std::string& path) {
  SAMPLE* temp_loader;
  if (!(temp_loader = load_sample(path.c_str())))
    abort_on_error(("Cannot find sound " + path +
                    "\nPlease check your files and try again")
                       .c_str());

  return temp_loader;
}

// Load and error check fonts
FONT* loadFont(const std::string& path) {
  FONT* temp_loader;
  if (!(temp_loader = load_font(path.c_str(), nullptr, nullptr)))
    abort_on_error(
        ("Cannot find font " + path + "\nPlease check your files and try again")
            .c_str());

  return extract_font_range(temp_loader, ' ', 'z');
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
