#include "InterfaceTypeManager.h"

#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../ui/UiLabel.h"
#include "../ui/UiSlot.h"
#include "../utility/Vec2.h"

std::vector<UiController> InterfaceTypeManager::ui_defs;

// Load interfaces
int InterfaceTypeManager::loadInterfaces(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

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
    auto controller = UiController(name, Vec2<int>(width, height));

    // Labels
    for (auto const& label : interface["labels"]) {
      std::string text = label["text"];
      int x = label["x"];
      int y = label["y"];
      controller.addElement(std::make_shared<UiLabel>(Vec2<int>(x, y), text));
    }

    // Slots
    for (auto const& slot : interface["slots"]) {
      int x = slot["x"];
      int y = slot["y"];
      std::string type = slot["type"];
      controller.addElement(std::make_shared<UiSlot>(Vec2<int>(x, y), type));
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
