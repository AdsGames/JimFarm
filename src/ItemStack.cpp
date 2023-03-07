#include "ItemStack.h"

ItemStack::ItemStack(std::shared_ptr<Item> item, int quantity) {
  setItem(item, quantity);
}

void ItemStack::setItem(std::shared_ptr<Item> item, int quantity) {
  this->item = item;
  this->quantity = quantity;
}

std::shared_ptr<Item> ItemStack::getItem() const {
  return item;
}

int ItemStack::getQuantity() const {
  return quantity;
}

void ItemStack::remove(int remove_quantity) {
  if (remove_quantity < this->quantity) {
    this->quantity -= remove_quantity;
  } else if (remove_quantity == this->quantity) {
    this->quantity = 0;
    this->item = nullptr;
  }
}

void ItemStack::add(int add_quantity) {
  if (add_quantity > 0 && item != nullptr) {
    this->quantity += add_quantity;
  }
}

void ItemStack::clear() {
  remove(this->quantity);
}

void ItemStack::draw(int x, int y) const {
  if (item == nullptr) {
    return;
  }

  item->draw(x, y);

  if (quantity > 1) {
    // asw::draw::textCenter(font, std::to_string(quantity), x + 10, y + 2,
    //                       asw::util::makeColor(255, 255, 255));
  }
}
