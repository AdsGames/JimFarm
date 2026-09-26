#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <asw/asw.h>
#include <memory>
#include <vector>

#include "../Inventory.h"
#include "UiElement.h"

constexpr int DRAG_BOX_HEIGHT = 10;

class UiController {
 public:
  UiController(const std::string& name, const asw::Vec2i& size);

  void draw();

  void update();

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

  std::string& getName();

 private:
  std::shared_ptr<UiElement> elementAt(const asw::Vec2i& pos) const;

  std::shared_ptr<Inventory> inv{nullptr};

  std::string name{""};

  asw::Vec2i size{0, 0};
  asw::Vec2i position{0, 0};

  std::vector<std::shared_ptr<UiElement>> elements{};

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};

  bool dragging{false};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
