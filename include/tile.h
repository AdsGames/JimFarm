#ifndef TILE_H
#define TILE_H

#include <allegro.h>

class tile
{
    public:
        tile(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, char newItemID = -1);
        virtual ~tile();

        void draw( BITMAP *tempBuffer);

        bool solid;

        // Positioning
        int x, y;

        // Item placed
        char item_id;

        // ID
        char id;

        // Images
        BITMAP *image[2];
        BITMAP *item_image;

    protected:

    private:
};

#endif // TILE_H
