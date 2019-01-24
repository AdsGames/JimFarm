#ifndef MAPITEM_H
#define MAPITEM_H

#include "Sprite.h"
#include "Item.h"

class MapItem : public Sprite {
  public:
    MapItem (int x = 0, int y = 0, Item *itemPtr = NULL);
    ~MapItem();

    virtual void draw (BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2) override;

    Item *itemPtr;
};

#endif // MAPITEM_H
