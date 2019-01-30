#include "Inventory.h"

Inventory::Inventory() {
  this -> max_items = 0;
}

Inventory::Inventory (int max_items) {
  this -> max_items = max_items;

  contents.reserve(max_items);
  emptyInv();
}

Inventory::~Inventory() {}

// Push item to contents (if it fits)
bool Inventory::addItem (Item *newItem, int index) {
  if (index < getMaxSize() && newItem != NULL) {
    contents[index] = newItem;
    return true;
  }
  return false;
}

// Remove item at index
bool Inventory::removeItem (int index) {
  if (getItem (index) != NULL) {
    contents[index] = NULL;
    return true;
  }
  return false;
}

// Gets item at index if exists
Item* Inventory::getItem (int index) {
  if (index < getMaxSize())
    return contents[index];
  return NULL;
}

// Just returns first item
Item* Inventory::getFirstItem() {
  return getItem(0);
}

// Current item count
int Inventory::getSize() {
  return contents.size();
}

// Just returns max size
int Inventory::getMaxSize() {
  return max_items;
}

// Clear all contents
void Inventory::emptyInv() {
  contents.clear();

  for (int i = 0; i < getMaxSize(); i++)
    contents.push_back (NULL);
}
