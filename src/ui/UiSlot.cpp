#include "UiSlot.h"

UiSlot::UiSlot(int x, int y) : UiElement(x, y, SLOT_SIZE, SLOT_SIZE) {}

UiSlot::UiSlot(int x, int y, std::shared_ptr<ItemStack> stk) : UiSlot(x, y) {
  bindStack(stk);
}

void UiSlot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;
}

std::shared_ptr<ItemStack> UiSlot::getStack() const {
  return this->stkptr;
}

void UiSlot::draw(int parent_x, int parent_y) {
  asw::draw::rectFill(getX() + parent_x, getY() + parent_y, SLOT_SIZE,
                      SLOT_SIZE, asw::util::makeColor(80, 80, 80));

  if (stkptr) {
    stkptr->draw(getX() + parent_x, getY() + parent_y);
  }
}
