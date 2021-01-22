#ifndef MAPITEM_H
#define MAPITEM_H

#include "Item.h"
#include "Sprite.h"

class MapItem : public Sprite {
 public:
  MapItem(int x, int y, Item* itemPtr);
  ~MapItem();

  virtual void draw(BITMAP* tempBuffer,
                    float x_1,
                    float y_1,
                    float x_2,
                    float y_2) override;

  Item* itemPtr;
};

#endif  // MAPITEM_H
