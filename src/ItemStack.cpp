#include "ItemStack.h"

ItemStack::ItemStack() : ItemStack(nullptr, 0) {}

ItemStack::ItemStack(Item* item, int quantity) {
  setItem(item, quantity);
}

ItemStack::~ItemStack() {}

void ItemStack::setItem(Item* item, int quantity) {
  this->item = item;
  this->quantity = quantity;
}

Item* ItemStack::getItem() {
  return item;
}

int ItemStack::getQuantity() {
  return quantity;
}

void ItemStack::remove(int quantity) {
  if (quantity < this->quantity) {
    this->quantity -= quantity;
  } else if (quantity == this->quantity) {
    this->quantity = 0;
    this->item = nullptr;
  }
}

void ItemStack::add(int quantity) {
  if (quantity > 0 && item != nullptr) {
    this->quantity += quantity;
  }
}

void ItemStack::clear() {
  remove(this->quantity);
}

void ItemStack::draw(int x, int y, BITMAP* buffer) {
  if (item != nullptr) {
    item->draw(x, y, buffer);
    if (quantity > 1)
      textprintf_centre_ex(buffer, font, x + 10, y + 2, makecol(255, 255, 255),
                           -1, "x%d", quantity);
  }
}
