#include "UI_Slot.h"

UI_Slot::UI_Slot(int x, int y) : UI_Element(x, y, SLOT_SIZE, SLOT_SIZE) {}

UI_Slot::UI_Slot(int x, int y, std::shared_ptr<ItemStack> stk) : UI_Slot(x, y) {
  bindStack(stk);
}

void UI_Slot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;
}

std::shared_ptr<ItemStack> UI_Slot::getStack() const {
  return this->stkptr;
}

void UI_Slot::draw(int parent_x, int parent_y) {
  asw::draw::rectFill(getX() + parent_x, getY() + parent_y, SLOT_SIZE,
                      SLOT_SIZE, asw::util::makeColor(80, 80, 80));

  if (stkptr) {
    stkptr->draw(getX() + parent_x, getY() + parent_y);
  }
}
