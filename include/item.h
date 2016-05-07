#ifndef ITEM_H
#define ITEM_H

#include <allegro.h>
#include "tile.h"

class item : public tile
{
    public:
        item(char newID, BITMAP *newImage);
        virtual ~item();
    protected:

    private:

};

#endif // ITEM_H
