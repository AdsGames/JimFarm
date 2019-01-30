#include "Tools.h"

#include <time.h>

int game_state = 0;

// Random number generator. Use int random(highest,lowest);
int random( int min, int max){
    return (rand() % (max + 1 - min)) + min;
}
// ERROR REPORTING
void abort_on_error(const char *message) {
	 if (screen != NULL) {
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}


// Load bitmap with error checked_array_iterator
BITMAP *load_bitmap_ex(const char *path) {
  BITMAP *temp_loader;
  if (!(temp_loader = load_bitmap(path, NULL)))
    abort_on_error(("Cannot find image " + std::string(path) + "\nPlease check your files and try again").c_str());

  return temp_loader;
}

// Load and error check sounds
SAMPLE *load_sample_ex(const char *path) {
  SAMPLE *temp_loader;
  if (!(temp_loader = load_sample(path)))
    abort_on_error(("Cannot find sound " + std::string(path) + "\nPlease check your files and try again").c_str());

  return temp_loader;
}

// Load and error check fonts
FONT *load_font_ex(const char *path) {
  FONT *temp_loader;
  if (!(temp_loader = load_font(path, NULL, NULL)))
    abort_on_error(("Cannot find font " + std::string(path) + "\nPlease check your files and try again").c_str());

  return extract_font_range(temp_loader, ' ', 'z');
}


//Collision
bool collision(int xMin1, int xMax1, int xMin2, int xMax2, int yMin1, int yMax1, int yMin2, int yMax2) {
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1) {
    return true;
  }
  return false;
}

// Convert string to int
int stoi(std::string text) {
  return atoi(text.c_str());
}
