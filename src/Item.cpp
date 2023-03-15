#include "Item.h"

#include "manager/ItemTypeManager.h"

// Ctor for item
Item::Item(const std::string& id, unsigned char meta)
    : meta(meta), item_pointer(ItemTypeManager::getItem(id)) {}

// Draw item to screen
void Item::draw(Vec2<int> position) const {
  item_pointer.draw(position.x, position.y, getMeta());
}

void Item::setMeta(unsigned char meta) {
  this->meta = meta;
}

void Item::changeMeta(unsigned char amt) {
  this->meta += amt;
}

unsigned char Item::getMeta() const {
  return this->meta;
}

const TileType& Item::getType() const {
  return item_pointer;
}