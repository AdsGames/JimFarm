#include "Item.h"

#include "manager/ItemTypeManager.h"

// Ctor for item
Item::Item(char id, unsigned char meta)
    : meta(meta), item_pointer(ItemTypeManager::getItemById(id)) {}

// Draw item to screen
void Item::draw(int x, int y) const {
  item_pointer.draw(x, y, getMeta());
}

// Get ID
unsigned char Item::getId() const {
  return item_pointer.getId();
}

// Get the name of item
std::string Item::getName() const {
  return item_pointer.getName();
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