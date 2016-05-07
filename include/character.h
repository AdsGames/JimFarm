#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>

#include "tile_map.h"



class character
{
    public:
        character();
        virtual ~character();

        void setPosition( int newX, int newY){ x = newX; y = newY;}
        void setImage( BITMAP *newImage){ image = newImage;}

        void draw( BITMAP *tempBuffer);

        void update();

        void setWorld( tile_map *newTileMap);
    protected:

    private:
        int x, y;
        BITMAP *image;

        char gameTick;
        char direction;

        tile_map *map_pointer;

};

#endif // CHARACTER_H
