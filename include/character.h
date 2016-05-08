#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>
#include <string>
#include <iostream>

#include "math.h"
#include "tile_map.h"
#include "tile.h"

#define MAX_MESSAGES 1

class character
{
    public:
        character();
        virtual ~character();

        void setPosition( int newX, int newY){ x = newX; y = newY;}
        void setImage( BITMAP *newImage);

        void draw( BITMAP *tempBuffer);
        void drawForeground( BITMAP *tempBuffer);

        void push_message( std::string new_message);

        void update();

        void remove_item();
        void give_item( char newItem);

        void setWorld( tile_map *newTileMap);

        item *inventory_item;
        item *inventory_hand;

        int money;

        bool store_open;
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

        int water;
        int sound_step;

        BITMAP *image;
        BITMAP *watering_can[5];
        BITMAP *inventory_gui;
        BITMAP *hand;
        BITMAP *indicator;
        BITMAP *coin;

        SAMPLE* pickup;
        SAMPLE* drop;
        SAMPLE* step_1;
        SAMPLE* step_2;
        SAMPLE* cut_axe;
        SAMPLE* cut_scythe;

        int indicator_x, indicator_y;

        char gameTick;
        char direction;
        bool moving;

        int tick;

        tile_map *map_pointer;

        std::string player_messages[MAX_MESSAGES];
        bool recent_message;

};

#endif // CHARACTER_H
