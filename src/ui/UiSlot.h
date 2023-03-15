#ifndef SRC_UI_UI_SLOT_H_
#define SRC_UI_UI_SLOT_H_

#include <asw/asw.h>

#include "../ItemStack.h"
#include "UiElement.h"

const int SLOT_SIZE = 16;

class UiSlot : public UiElement {
 public:
  explicit UiSlot(Vec2<int> pos);
  UiSlot(Vec2<int> pos, std::shared_ptr<ItemStack> stk);
  ~UiSlot() final = default;

  void bindStack(std::shared_ptr<ItemStack> stk);

  std::shared_ptr<ItemStack> getStack() const;

  void draw(Vec2<int> parent_pos) override;

 private:
  std::shared_ptr<ItemStack> stkptr{nullptr};
};

#endif  // SRC_UI_UI_SLOT_H_
