#include "UiSlot.h"

UiSlot::UiSlot(Vec2<int> pos, std::string type)
    : UiElement(pos, Vec2<int>(SLOT_SIZE, SLOT_SIZE)), type(type) {}

void UiSlot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;
}

std::shared_ptr<ItemStack> UiSlot::getStack() const {
  return this->stkptr;
}

void UiSlot::draw(Vec2<int> parent_pos) {
  auto draw_pos = parent_pos + getPosition();

  asw::draw::rectFill(draw_pos.x, draw_pos.y, SLOT_SIZE, SLOT_SIZE,
                      asw::util::makeColor(110, 110, 110));

  asw::draw::rectFill(draw_pos.x + 1, draw_pos.y + 1, SLOT_SIZE - 2,
                      SLOT_SIZE - 2, asw::util::makeColor(80, 80, 80));

  if (stkptr) {
    stkptr->draw(draw_pos);
  }
}

const std::string& UiSlot::getType() const {
  return type;
}