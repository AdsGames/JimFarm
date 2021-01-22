#include "UI_Slot.h"

UI_Slot::UI_Slot(int x, int y) : UI_Element(x, y, SLOT_SIZE, SLOT_SIZE) {
  BindStack(nullptr);
}

UI_Slot::UI_Slot(int x, int y, ItemStack* stk) : UI_Slot(x, y) {
  BindStack(stk);
}

void UI_Slot::BindStack(ItemStack* stk) {
  this->stkptr = stk;
}

ItemStack* UI_Slot::GetStack() {
  return this->stkptr;
}

void UI_Slot::Draw(BITMAP* buffer, int parent_x, int parent_y) {
  rectfill(buffer, GetX() + parent_x, GetY() + parent_y,
           GetX() + parent_x + SLOT_SIZE, GetY() + parent_y + SLOT_SIZE,
           makecol(80, 80, 80));
  if (stkptr) {
    stkptr->Draw(GetX() + parent_x, GetY() + parent_y, buffer);
  }
}
