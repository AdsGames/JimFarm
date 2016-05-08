#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>
#include <iostream>

#include "tools.h"

class tile
{
    public:
        tile(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID);
        virtual ~tile();

        void draw( BITMAP *tempBuffer);

        bool solid;

        // Positioning
        int x, y;

        // ID
        char id;

        // Images
        BITMAP *image[2];

        // Sorting stuff
        bool operator< (const tile &other) const {
            return y < other.y;
        }

        virtual void run_tick();
        bool requirements_met;

    protected:

    private:
};

class item : public tile
{
    public:
        item(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, std::string newName);
        virtual ~item();

        std::string name;

        char value;
    protected:

    private:

};

class crop : public tile
{
    public:
        crop(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, std::string newName);
        virtual ~crop();

        std::string name;
        char growth_tick;

        virtual void run_tick();
    protected:

    private:

};

#endif // TILE_H
