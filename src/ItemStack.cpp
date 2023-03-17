#include "ItemStack.h"

asw::Font ItemStack::pixelart = nullptr;

ItemStack::ItemStack() : ItemStack(nullptr, 0) {}

ItemStack::ItemStack(std::shared_ptr<Item> item, int quantity)
    : item(item), quantity(quantity) {
  if (!pixelart) {
    pixelart = asw::assets::loadFont("assets/fonts/pixelart.ttf", 8);
  }
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

void ItemStack::draw(Vec2<int> position) const {
  if (item == nullptr) {
    return;
  }

  item->draw(position);

  if (quantity > 1) {
    asw::draw::textCenter(pixelart, std::to_string(quantity), position.x + 13,
                          position.y + 9, asw::util::makeColor(0, 0, 0));
    asw::draw::textCenter(pixelart, std::to_string(quantity), position.x + 12,
                          position.y + 8, asw::util::makeColor(255, 255, 255));
  }
}
