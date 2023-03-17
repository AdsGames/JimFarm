#include "MapItem.h"

MapItem::MapItem(Vec2<int> position, std::shared_ptr<Item> itemPtr)
    : Sprite(position, 2), itemPtr(itemPtr) {}

void MapItem::draw(const Camera& camera) const {
  itemPtr->draw(pos - camera.getPosition());
}
