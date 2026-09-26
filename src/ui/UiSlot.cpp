#include "UiSlot.h"

UiSlot::UiSlot(const asw::Vec2i& pos, std::string type)
    : UiElement(pos, asw::Vec2i(SLOT_SIZE, SLOT_SIZE)), type(type) {}

void UiSlot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;
}

std::shared_ptr<ItemStack> UiSlot::getStack() const {
  return this->stkptr;
}

void UiSlot::draw(const asw::Vec2i& parent_pos) {
  auto draw_pos = parent_pos + getPosition();

  asw::draw::rect_fill(asw::Quadf(draw_pos.x, draw_pos.y, SLOT_SIZE, SLOT_SIZE),
                       asw::Color(110, 110, 110));

  asw::draw::rect_fill(
      asw::Quadf(draw_pos.x + 1, draw_pos.y + 1, SLOT_SIZE - 2, SLOT_SIZE - 2),
      asw::Color(80, 80, 80));

  if (stkptr) {
    stkptr->draw(draw_pos);
  }
}

const std::string& UiSlot::getType() const {
  return type;
}