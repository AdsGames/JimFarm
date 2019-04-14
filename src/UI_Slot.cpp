#include "UI_Slot.h"

UI_Slot::UI_Slot(int x, int y) :
  UI_Element(x, y, SLOT_SIZE, SLOT_SIZE) {
  item = nullptr;
}

UI_Slot::UI_Slot(int x, int y, Item *item) :
  UI_Slot(x, y) {
  BindItem(item);
}

void UI_Slot::BindItem(Item *item) {
  this -> item = item;
}

Item* UI_Slot::GetItem() {
  return this -> item;
}

void UI_Slot::Draw(BITMAP *buffer, int parent_x, int parent_y) {
  rectfill(buffer, GetX() + parent_x, GetY() + parent_y, GetX() + parent_x + SLOT_SIZE, GetY() + parent_y + SLOT_SIZE, makecol(80, 80, 80));
  if (item != nullptr) {
    item -> draw (GetX() + parent_x, GetY() + parent_y, buffer);
  }
}
