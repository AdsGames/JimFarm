#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>
#include <iostream>

#include "tools.h"
#include "tile_type_manager.h"

class tile
{
    public:
        tile(int newX, int newY, char newID);
        virtual ~tile();

        // Positioning
        int x, y;
        int getWidth(){ return tile_pointer -> getWidth();}
        int getHeight(){ return tile_pointer -> getHeight();}

        // Drawing
        void draw( BITMAP *tempBuffer);
        void draw_at( int newX, int newY, BITMAP *tempBuffer);

        // Sorting stuff
        bool operator< (const tile &other) const {
            return y < other.y;
        }

        virtual void run_tick();
        bool requirements_met;

        tile_type *tile_pointer;

        bool isSolid(){ return tile_pointer -> getAttribute(); }

        unsigned char getID(){ return tile_pointer -> getID();}
        void setID( unsigned char newID);

        std::string getName(){ return tile_pointer -> getName();}
    protected:

    private:
};

class item : public tile
{
    public:
        item(int newX, int newY, char newID);
        virtual ~item();

        void draw( BITMAP *tempBuffer);

        unsigned char getValue(){ return tile_pointer -> getValue();}
    protected:

    private:

};

class crop : public tile
{
    public:
        crop(int newX, int newY, char newID);
        virtual ~crop();

        char growth_tick;

        virtual void run_tick();
    protected:

    private:

};

#endif // TILE_H
