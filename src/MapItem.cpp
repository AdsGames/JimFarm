#include "MapItem.h"

MapItem::MapItem(int x, int y, std::shared_ptr<Item> itemPtr)
    : Sprite(x, y, 3), itemPtr(itemPtr) {}

void MapItem::draw(int x_1, int y_1, int x_2, int y_2) const {
  if (x >= x_1 && x <= x_2 && y >= y_1 && y <= y_2) {
    itemPtr->draw(x - x_1, y - y_1);
  }
}
