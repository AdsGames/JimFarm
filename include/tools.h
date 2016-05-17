#ifndef TOOLS_H
#define TOOLS_H
#define BACKGROUND false
#define FOREGROUND true
#define MAIN_MENU 0
#define HELP 1
#define OPTIONS 2
#define STORY 3

#include <allegro.h>
#include <time.h>
#include <string>



// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const char *message);

// Load and error check images
extern BITMAP *load_bitmap_ex( const char *path);

// Load and error check sounds
extern SAMPLE *load_sample_ex( const char *path);

// Load and error check fonts
extern FONT *load_font_ex( const char *path);

#endif // TOOLS_H
