#include "tile.h"

tile::tile(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, char newItemID)
{
    // Set init variables
    x = newX;
    y = newY;

    image[0] = newImage1;
    image[1] = newImage2;

    id = newID;
    item_id = newItemID;
    item_image = NULL;
}

tile::~tile()
{
    //dtor
}

void tile::draw( BITMAP *tempBuffer)
{
    draw_sprite( tempBuffer, image[0], x, y);
    if( item_image != NULL)
        draw_sprite( tempBuffer, item_image, x, y);
}
