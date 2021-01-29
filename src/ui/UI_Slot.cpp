#include "UI_Slot.h"

UI_Slot::UI_Slot(int x, int y) : UI_Element(x, y, SLOT_SIZE, SLOT_SIZE) {
  bindStack(nullptr);
}

UI_Slot::UI_Slot(int x, int y, ItemStack* stk) : UI_Slot(x, y) {
  bindStack(stk);
}

void UI_Slot::bindStack(ItemStack* stk) {
  this->stkptr = stk;
}

ItemStack* UI_Slot::getStack() {
  return this->stkptr;
}

void UI_Slot::draw(BITMAP* buffer, int parent_x, int parent_y) {
  rectfill(buffer, getX() + parent_x, getY() + parent_y,
           getX() + parent_x + SLOT_SIZE, getY() + parent_y + SLOT_SIZE,
           makecol(80, 80, 80));
  if (stkptr) {
    stkptr->draw(getX() + parent_x, getY() + parent_y, buffer);
  }
}
