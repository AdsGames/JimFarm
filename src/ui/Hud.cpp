#include "Hud.h"

#include "../manager/InterfaceTypeManager.h"

Hud::Hud() {}

void Hud::draw() {
  for (auto& [_key, ui] : ui_controllers) {
    ui.draw();
  }
}

void Hud::toggleUiController(const std::string& name,
                             const UiController& ui_controller) {
  if (ui_controllers.contains(name)) {
    ui_controllers.erase(name);
    return;
  }

  ui_controllers.insert({name, ui_controller});
}

void Hud::update() {
  for (auto& [_, ui] : ui_controllers) {
    ui.update();
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_E]) {
    this->toggleUiController(
        "inventory", InterfaceTypeManager::getInterfaceByName("inventory"));
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_Q]) {
    this->toggleUiController(
        "crafting", InterfaceTypeManager::getInterfaceByName("crafting"));
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_G]) {
    this->toggleUiController(
        "furnace", InterfaceTypeManager::getInterfaceByName("furnace"));
  }
}