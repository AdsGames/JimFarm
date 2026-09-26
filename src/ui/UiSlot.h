#ifndef SRC_UI_UI_SLOT_H_
#define SRC_UI_UI_SLOT_H_

#include <asw/asw.h>
#include <string>

#include "../ItemStack.h"
#include "UiElement.h"

const int SLOT_SIZE = 16;

// What a slot does when clicked
enum class SlotType {
  // Holds items
  Input,

  // Crafting result, taking it uses the inputs
  Output,

  // Store stock (item_id), click to buy one
  Buy,

  // Drop items to sell them
  Sell,

  // Drop items to fill the store order
  Order,
};

// Slot type from its interfaces.json name, throws on unknown names
SlotType slotTypeFromName(const std::string& name);

class UiSlot : public UiElement {
 public:
  UiSlot(const asw::Vec2i& pos, SlotType type, std::string item_id = "");

  void bindStack(std::shared_ptr<ItemStack> stk);

  std::shared_ptr<ItemStack> getStack() const;

  void draw(const asw::Vec2i& parent_pos, const GameState& state) override;

  SlotType getType() const;

  const std::string& getItemId() const;

 private:
  std::shared_ptr<ItemStack> stkptr{nullptr};

  SlotType type{SlotType::Input};

  std::string item_id{""};
};

#endif  // SRC_UI_UI_SLOT_H_
