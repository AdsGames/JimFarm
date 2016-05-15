#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include <vector>

#include "tile.h"
#include "tools.h"
#include "character.h"

class menu
{
    public:
        menu();
        virtual ~menu();

        void draw( BITMAP *tempBitmap);

        void load_data();

        void update();

    protected:

    private:

        int tick;

        BITMAP *menu_image;
        BITMAP *story_image;
        BITMAP *help_image;

        int state;

};

#endif // MENU_H
