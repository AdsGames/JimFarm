#ifndef TOOLS_H
#define TOOLS_H
#define BACKGROUND false
#define FOREGROUND true

#include <allegro.h>
#include<time.h>



// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const char *message);


#endif // TOOLS_H
