#ifndef SRC_MAPITEM_H_
#define SRC_MAPITEM_H_

#include "Item.h"
#include "Sprite.h"

class MapItem : public Sprite {
 public:
  MapItem(int x, int y, std::shared_ptr<Item> itemPtr);
  ~MapItem() final = default;

  void draw(int x_1, int y_1, int x_2, int y_2) const override;

  std::shared_ptr<Item> itemPtr;
};

#endif  // SRC_MAPITEM_H_
