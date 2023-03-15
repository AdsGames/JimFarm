#include "MapItem.h"

MapItem::MapItem(Vec2<int> position, std::shared_ptr<Item> itemPtr)
    : Sprite(position, 3), itemPtr(itemPtr) {}

void MapItem::draw(const Camera& camera) const {
  if (camera.getBounds().contains(pos)) {
    itemPtr->draw(pos - camera.getPosition());
  }
}
