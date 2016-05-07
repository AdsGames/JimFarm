#ifndef TILE_H
#define TILE_H

#include <allegro.h>

class tile
{
    public:
        tile(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID);
        virtual ~tile();

        void draw( BITMAP *tempBuffer);

        bool solid;

    protected:

    private:
        // Images
        BITMAP *image[2];

        // Positioning
        int x, y;

        // ID
        char id;

};

#endif // TILE_H
