#include "Hud.h"

#include "../manager/InterfaceTypeManager.h"
#include "UiScale.h"

Hud::Hud() {}

void Hud::draw(const GameState& state) {
  for (auto& [_key, ui] : ui_controllers) {
    ui.draw(state);
  }
}

void Hud::toggleUiController(const std::string& name,
                             const UiController& ui_controller) {
  if (ui_controllers.contains(name)) {
    ui_controllers.erase(name);
    returnMouseItem();
    return;
  }

  ui_controllers.insert({name, ui_controller});
}

void Hud::open(const std::string& name, float vertical) {
  if (ui_controllers.contains(name)) {
    return;
  }

  auto controller = InterfaceTypeManager::getInterfaceByName(name);

  if (vertical >= 0.0F) {
    const auto free = getUiSize() - controller.getSize();
    controller.setPosition(asw::Vec2i(
        free.x / 2, DRAG_BOX_HEIGHT + static_cast<int>(free.y * vertical)));
  }

  ui_controllers.insert({name, controller});
}

void Hud::closeAll() {
  ui_controllers.clear();
  returnMouseItem();
}

void Hud::returnMouseItem() {
  if (ui_controllers.empty()) {
    UiController::returnMouseItem(
        *InterfaceTypeManager::getInterfaceByName("inventory").getInventory());
  }
}

void Hud::update(GameState& state) {
  for (auto& [_, ui] : ui_controllers) {
    ui.update(state);
  }

  if (asw::input::get_key_down(asw::input::Key::E)) {
    this->toggleUiController(
        "inventory", InterfaceTypeManager::getInterfaceByName("inventory"));
  }

  if (asw::input::get_key_down(asw::input::Key::Q)) {
    this->toggleUiController(
        "crafting", InterfaceTypeManager::getInterfaceByName("crafting"));
  }

  if (asw::input::get_key_down(asw::input::Key::G)) {
    this->toggleUiController(
        "furnace", InterfaceTypeManager::getInterfaceByName("furnace"));
  }
}
