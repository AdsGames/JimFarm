#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>
#include <string>

#include "math.h"
#include "tile_map.h"

#define MAX_MESSAGES 1

class character
{
    public:
        character();
        virtual ~character();

        void setPosition( int newX, int newY){ x = newX; y = newY;}
        void setImage( BITMAP *newImage);

        void draw( BITMAP *tempBuffer);

        void push_message( std::string new_message);

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
        BITMAP *watering_can[5];

        char gameTick;
        char direction;
        bool moving;

        tile_map *map_pointer;

        std::string player_messages[MAX_MESSAGES];
        bool recent_message;

};

#endif // CHARACTER_H
