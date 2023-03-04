#include "Item.h"

#include "manager/ItemTypeManager.h"

// Ctor for item
Item::Item(char newID, unsigned char meta) {
  this->item_pointer = ItemTypeManager::getItemById(newID);
  this->meta = meta;
}

// Draw item to screen
void Item::draw(int x, int y) {
  if (item_pointer)
    item_pointer->draw(x, y, getMeta());
}

// Get value of item
unsigned char Item::getValue() const {
  if (item_pointer)
    return item_pointer->getValue();
  return 0;
}

// Get ID
unsigned char Item::getID() const {
  if (item_pointer)
    return item_pointer->getID();
  return 0;
}

// Get the name of item
std::string Item::getName() const {
  if (item_pointer)
    return item_pointer->getName();
  return "null item";
}
