#include "Inventory.h"

// Push item to contents (if it fits)
bool Inventory::addItem(std::shared_ptr<Item> item, int quantity) {
  // Null item
  if (item == nullptr) {
    return false;
  }

  // Stack
  auto stk = findStack(item);
  if (stk != nullptr) {
    stk->add(quantity);
    return true;
  }

  // Find a new spot
  int emptyStk = findFirstEmpty();
  if (emptyStk != -1) {
    contents[emptyStk]->setItem(item, quantity);
    return true;
  }

  return false;
}

// Remove item at index
bool Inventory::removeItem(int index) {
  if (getStack(index) != nullptr) {
    contents[index]->clear();
    return true;
  }
  return false;
}

// Add a space
void Inventory::addSpace() {
  contents.push_back(std::make_shared<ItemStack>());
}

// Gets item at index if exists
std::shared_ptr<ItemStack> Inventory::getStack(int index) {
  if (index < getSize() && index >= 0) {
    return contents[index];
  }

  return nullptr;
}

// Just returns first item
std::shared_ptr<ItemStack> Inventory::getFirstItem() {
  return getStack(0);
}

// Find stack
std::shared_ptr<ItemStack> Inventory::findStack(
    std::shared_ptr<Item> item) const {
  for (auto const& content : contents) {
    if (content && content->getItem()) {
      if (content->getItem()->getType().getId() == item->getType().getId()) {
        return content;
      }
    }
  }
  return nullptr;
}

// Get first empty
int Inventory::findFirstEmpty() {
  for (unsigned int i = 0; i < contents.size(); i++) {
    if (contents.at(i) && !(contents.at(i)->getItem())) {
      return i;
    }
  }
  return -1;
}

// Current item count
int Inventory::getSize() const {
  return contents.size();
}

// Clear all contents
void Inventory::empty() {
  contents.clear();

  for (int i = 0; i < getSize(); i++) {
    contents.push_back(std::make_shared<ItemStack>());
  }
}
