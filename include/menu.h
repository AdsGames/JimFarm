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

        bool update();

    protected:

    private:

        int tick;
        int coin_frame;
        bool coin_direction;
        int indicator_location=3;

        BITMAP *menu_image;
        BITMAP *story_image;
        BITMAP *help_image;
        BITMAP *coin_flip;

        int state;

};

#endif // MENU_H
