#include "UiSlot.h"

#include <format>
#include <stdexcept>

#include "../GameState.h"
#include "../manager/ItemTypeManager.h"

SlotType slotTypeFromName(const std::string& name) {
  if (name == "input") {
    return SlotType::Input;
  }
  if (name == "output") {
    return SlotType::Output;
  }
  if (name == "buy") {
    return SlotType::Buy;
  }
  if (name == "sell") {
    return SlotType::Sell;
  }
  if (name == "order") {
    return SlotType::Order;
  }

  throw std::runtime_error("Unknown slot type: " + name);
}

UiSlot::UiSlot(const asw::Vec2i& pos, SlotType type, std::string item_id)
    : UiElement(pos, asw::Vec2i(SLOT_SIZE, SLOT_SIZE)),
      type(type),
      item_id(item_id) {}

void UiSlot::bindStack(std::shared_ptr<ItemStack> stk) {
  this->stkptr = stk;

  // Store stock shows its item
  if (type == SlotType::Buy && !item_id.empty() && stkptr) {
    stkptr->setItem(std::make_shared<Item>(item_id), 1);
  }
}

std::shared_ptr<ItemStack> UiSlot::getStack() const {
  return this->stkptr;
}

void UiSlot::draw(const asw::Vec2i& parent_pos, const GameState& state) {
  auto draw_pos = parent_pos + getPosition();

  // Border colour shows what the slot does
  auto border = asw::Color(110, 110, 110);
  switch (type) {
    case SlotType::Sell:
      border = asw::Color(230, 190, 40);
      break;
    case SlotType::Order:
      border = asw::Color(80, 170, 230);
      break;
    case SlotType::Output:
      border = asw::Color(150, 200, 120);
      break;
    case SlotType::Input:
    case SlotType::Buy:
      break;
  }

  asw::draw::rect_fill(asw::Quadf(draw_pos.x, draw_pos.y, SLOT_SIZE, SLOT_SIZE),
                       border);

  asw::draw::rect_fill(
      asw::Quadf(draw_pos.x + 1, draw_pos.y + 1, SLOT_SIZE - 2, SLOT_SIZE - 2),
      asw::Color(80, 80, 80));

  if (type == SlotType::Order) {
    // Shows what the order wants
    if (state.order.active) {
      Item(state.order.item_id).draw(draw_pos);
      asw::draw::text(font,
                      std::format("{}/{}", state.order.delivered,
                                  state.order.quantity),
                      asw::Vec2f(draw_pos.x + SLOT_SIZE / 2, draw_pos.y + 17),
                      asw::Color(255, 255, 255), asw::TextJustify::Center);
    }
    return;
  }

  if (stkptr) {
    stkptr->draw(draw_pos);
  }

  // Price under store stock
  if (type == SlotType::Buy && !item_id.empty()) {
    const int price = ItemTypeManager::getInfo(item_id).price;
    asw::draw::text(font, std::format("${}", price),
                    asw::Vec2f(draw_pos.x + SLOT_SIZE / 2, draw_pos.y + 17),
                    state.getCoins() >= price ? asw::Color(255, 230, 120)
                                              : asw::Color(200, 120, 120),
                    asw::TextJustify::Center);
  }
}

SlotType UiSlot::getType() const {
  return type;
}

const std::string& UiSlot::getItemId() const {
  return item_id;
}
