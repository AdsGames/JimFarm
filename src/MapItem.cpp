#include "MapItem.h"

MapItem::MapItem(int x, int y, std::shared_ptr<Item> itemPtr)
    : Sprite(x, y, 3), itemPtr(itemPtr) {}

void MapItem::draw(const Camera& camera) const {
  if (camera.getBounds().contains(x, y)) {
    itemPtr->draw(x - camera.getX(), y - camera.getY());
  }
}
