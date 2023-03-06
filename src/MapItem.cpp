#include "MapItem.h"

MapItem::MapItem(int x, int y, std::shared_ptr<Item> itemPtr)
    : Sprite(x, y, 3), itemPtr(itemPtr) {}

void MapItem::draw(float x_1, float y_1, float x_2, float y_2) {
  if (x >= x_1 && x <= x_2 && y >= y_1 && y <= y_2) {
    itemPtr->draw(x - x_1, y - y_1);
  }
}
