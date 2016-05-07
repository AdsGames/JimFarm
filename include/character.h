#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>

#include "math.h"
#include "tile_map.h"



class character
{
    public:
        character();
        virtual ~character();

        void setPosition( int newX, int newY){ x = newX; y = newY;}
        void setImage( BITMAP *newImage);

        void draw( BITMAP *tempBuffer);

        void update();

        void setWorld( tile_map *newTileMap);
    protected:

    private:

        // Fonts
        FONT* f1;
        FONT* f2;
        FONT* f3;
        FONT* f4;
        FONT* f5;
        FONT* pixelart;

        int x, y;
        int inventory;
        int money;
        int water;

        BITMAP *image;

        char gameTick;
        char direction;
        bool moving;

        tile_map *map_pointer;

};

#endif // CHARACTER_H
