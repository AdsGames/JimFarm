#include "Item.h"

#include "manager/ItemTypeManager.h"

// Ctor for item
Item::Item(char newID, unsigned char meta) {
  this->item_pointer = ItemTypeManager::getItemById(newID);
  this->meta = meta;
}

// Dtor
Item::~Item() {}

// Draw item to screen
void Item::draw(int x, int y, BITMAP* buffer) {
  if (item_pointer)
    item_pointer->draw(x, y, buffer, getMeta());
}

// Get value of item
unsigned char Item::getValue() {
  if (item_pointer)
    return item_pointer->getValue();
  return 0;
}

// Get ID
unsigned char Item::getID() {
  if (item_pointer)
    return item_pointer->getID();
  return 0;
}

// Get the name of item
std::string Item::getName() {
  if (item_pointer)
    return item_pointer->getName();
  return "null item";
}
