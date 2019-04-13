#include "Inventory.h"

// Default constructor
Inventory::Inventory() :
  Inventory(0) {

}

// Constructor
Inventory::Inventory (int max_items) {
  this -> max_items = max_items;

  contents.reserve(max_items);
  emptyInv();
}

Inventory::~Inventory() {}

// Push item to contents (if it fits)
bool Inventory::addItem (Item *item, int index) {
  // Null item
  if (item == nullptr)
    return false;

  // Out of bounds
  if (index >= getMaxSize() || index < 0)
    return false;

  // Ensure space not taken
  if (contents[index] == nullptr) {
    contents[index] = item;
    return true;
  }
  return false;
}

// Remove item at index
bool Inventory::removeItem (int index) {
  if (getItem(index) != nullptr) {
    contents[index] = nullptr;
    return true;
  }
  return false;
}

// Gets item at index if exists
Item* Inventory::getItem (int index) {
  if (index < getMaxSize() && index >= 0)
    return contents[index];
  return nullptr;
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
    contents.push_back (nullptr);
}
