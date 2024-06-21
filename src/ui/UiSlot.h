#ifndef SRC_UI_UI_SLOT_H_
#define SRC_UI_UI_SLOT_H_

#include <asw/asw.h>
#include <string>

#include "../ItemStack.h"
#include "UiElement.h"

const int SLOT_SIZE = 16;

class UiSlot : public UiElement {
 public:
  UiSlot(Vec2<int> pos, std::string type = "");

  void bindStack(std::shared_ptr<ItemStack> stk);

  std::shared_ptr<ItemStack> getStack() const;

  void draw(Vec2<int> parent_pos) override;

  const std::string& getType() const;

 private:
  std::shared_ptr<ItemStack> stkptr{nullptr};

  std::string type{""};
};

#endif  // SRC_UI_UI_SLOT_H_
