#include "item.h"

item::item(char newID, BITMAP *newImage)
{
    id = newID;
    image = newImage;
}

item::~item()
{
    //dtor
}
