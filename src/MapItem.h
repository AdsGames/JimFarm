#ifndef SRC_MAPITEM_H_
#define SRC_MAPITEM_H_

#include "Item.h"
#include "Sprite.h"

class MapItem : public Sprite {
 public:
  MapItem(int x, int y, std::shared_ptr<Item> itemPtr);
  ~MapItem() final = default;

  void draw(const Camera& camera) const override;

  std::shared_ptr<Item> itemPtr;
};

#endif  // SRC_MAPITEM_H_
