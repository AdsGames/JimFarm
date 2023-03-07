#ifndef SRC_UI_UI_SLOT_H_
#define SRC_UI_UI_SLOT_H_

#include <asw/asw.h>

#include "../ItemStack.h"
#include "UiElement.h"

const int SLOT_SIZE = 16;

class UiSlot : public UiElement {
 public:
  UiSlot(int x, int y);
  UiSlot(int x, int y, std::shared_ptr<ItemStack> stk);
  ~UiSlot() final = default;

  void bindStack(std::shared_ptr<ItemStack> stk);

  std::shared_ptr<ItemStack> getStack() const;

  void draw(int parent_x, int parent_y) override;

 private:
  std::shared_ptr<ItemStack> stkptr{nullptr};
};

#endif  // SRC_UI_UI_SLOT_H_
