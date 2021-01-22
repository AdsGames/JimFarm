#ifndef UI_SLOT_H
#define UI_SLOT_H

#define SLOT_SIZE 16

#include <allegro.h>

#include "ItemStack.h"
#include "UI_Element.h"

class UI_Slot : public UI_Element {
 public:
  UI_Slot(int x, int y);
  UI_Slot(int x, int y, ItemStack* stk);
  virtual ~UI_Slot(){};

  void BindStack(ItemStack* stk);

  ItemStack* GetStack();

  virtual void Draw(BITMAP* buffer, int parent_x, int parent_y) override;

 protected:
 private:
  ItemStack* stkptr;
};

#endif  // UI_SLOT_H
