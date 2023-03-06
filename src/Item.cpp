#include "Item.h"

#include "manager/ItemTypeManager.h"

// Ctor for item
Item::Item(char newID, unsigned char meta)
    : meta(meta), item_pointer(ItemTypeManager::getItemById(newID)) {}

// Draw item to screen
void Item::draw(int x, int y) const {
  item_pointer.draw(x, y, getMeta());
}

// Get value of item
unsigned char Item::getValue() const {
  return item_pointer.getValue();
}

// Get ID
unsigned char Item::getID() const {
  return item_pointer.getID();
}

// Get the name of item
std::string Item::getName() const {
  return item_pointer.getName();
}
