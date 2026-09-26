#include "InterfaceTypeManager.h"

#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../ui/UiLabel.h"
#include "../ui/UiSlot.h"

std::vector<UiController> InterfaceTypeManager::ui_defs;

// Load interfaces
int InterfaceTypeManager::loadInterfaces(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  ui_defs.clear();

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (auto const& interface : doc) {
    // Name of interface
    std::string name = interface["name"];

    // Width and Height
    int width = interface["width"];
    int height = interface["height"];

    // Create ui controller
    auto controller = UiController(name, asw::Vec2i(width, height));

    // Labels
    for (auto const& label : interface.value("labels", nlohmann::json::array())) {
      std::string text = label.value("text", "");
      int x = label["x"];
      int y = label["y"];
      controller.addElement(std::make_shared<UiLabel>(
          asw::Vec2i(x, y), text, label.value("station", false)));
    }

    // Slots
    for (auto const& slot : interface.value("slots", nlohmann::json::array())) {
      int x = slot["x"];
      int y = slot["y"];
      const auto type = slotTypeFromName(slot["type"]);
      std::string item_id = slot.value("item_id", "");
      controller.addElement(
          std::make_shared<UiSlot>(asw::Vec2i(x, y), type, item_id));
    }

    // Push to controllers
    ui_defs.push_back(controller);
  }

  // Close
  file.close();
  return 0;
}

// Get interfaces by ID
UiController& InterfaceTypeManager::getInterfaceById(int id) {
  if (id >= 0 && id < (signed)ui_defs.size()) {
    return ui_defs.at(id);
  }

  // Throw error
  throw std::runtime_error("Interface not found");
}

// Get interfaces by Name
UiController& InterfaceTypeManager::getInterfaceByName(
    const std::string& name) {
  for (auto& controller : ui_defs) {
    if (controller.getName() == name) {
      return controller;
    }
  }

  // Throw error
  throw std::runtime_error("Interface not found");
}
