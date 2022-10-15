#ifndef SRC_UI_UI_SLOT_H_
#define SRC_UI_UI_SLOT_H_

#define SLOT_SIZE 16

#include <asw/asw.h>

#include "../ItemStack.h"
#include "UI_Element.h"

class UI_Slot : public UI_Element {
 public:
  UI_Slot(int x, int y);
  UI_Slot(int x, int y, ItemStack* stk);
  virtual ~UI_Slot() {}

  void bindStack(ItemStack* stk);

  ItemStack* getStack();

  void draw(int parent_x, int parent_y) override;

 protected:
 private:
  ItemStack* stkptr{nullptr};
};

#endif  // SRC_UI_UI_SLOT_H_
