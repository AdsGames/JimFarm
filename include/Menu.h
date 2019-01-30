#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include <vector>

#include "Tile.h"
#include "Tools.h"
#include "Character.h"

#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"

class Menu {
    public:
        Menu();
        virtual ~Menu();

        void draw (BITMAP *tempBitmap);

        void load_data();

        int update();

    protected:

    private:

        void draw_slider(BITMAP *tempBitmap, int x, int y, int value, std::string title);

        int tick;
        int coin_frame;
        bool coin_direction;
        int indicator_location;
        int music_volume;

        // Fonts
        FONT* f1;
        FONT* f2;
        FONT* f3;
        FONT* f4;
        FONT* f5;
        FONT* pixelart;

        BITMAP *menu_image;
        BITMAP *story_image;
        BITMAP *help_image;
        BITMAP *options_image;
        BITMAP *coin_flip;
        BITMAP *options_slider;
        BITMAP *options_slidee;
        BITMAP *options_indicator;

        SAMPLE *blip;

        int state;

};

#endif // MENU_H
