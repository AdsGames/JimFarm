#ifndef TOOLS_H
#define TOOLS_H
#define BACKGROUND false
#define FOREGROUND true
#define MAIN_MENU 0
#define HELP 1
#define OPTIONS 2
#define STORY 3

#include <allegro.h>
#include<time.h>



// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const char *message);


#endif // TOOLS_H
