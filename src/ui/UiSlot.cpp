#include "UiSlot.h"

UiSlot::UiSlot(Vec2<int> pos)
    : UiElement(pos, Vec2<int>(SLOT_SIZE, SLOT_SIZE)) {}

UiSlot::UiSlot(Vec2<int> pos, std::shared_ptr<ItemStack> stk) : UiSlot(pos) {
  bindStack(stk);
}

void UiSlot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;
}

std::shared_ptr<ItemStack> UiSlot::getStack() const {
  return this->stkptr;
}

void UiSlot::draw(Vec2<int> parent_pos) {
  auto draw_pos = parent_pos + getPosition();

  asw::draw::rectFill(draw_pos.x, draw_pos.y, SLOT_SIZE, SLOT_SIZE,
                      asw::util::makeColor(80, 80, 80));

  if (stkptr) {
    stkptr->draw(draw_pos);
  }
}
