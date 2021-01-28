#include "InterfaceTypeManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../utility/Tools.h"

#include "../ui/UI_Label.h"
#include "../ui/UI_Slot.h"

std::vector<UI_Controller*> InterfaceTypeManager::ui_defs;

// Destructor
InterfaceTypeManager::~InterfaceTypeManager() {
  ui_defs.clear();
}

// Load interfaces
int InterfaceTypeManager::load_interfaces(std::string path) {
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
    UI_Controller* controller = new UI_Controller(width, height);

    // Labels
    for (auto const& label : interface["labels"]) {
      std::string text = label["text"];
      int x = label["x"];
      int y = label["y"];
      controller->AddElement(new UI_Label(x, y, text));
    }

    // Slots
    for (auto const& slot : interface["slots"]) {
      int x = slot["x"];
      int y = slot["y"];
      controller->AddElement(new UI_Slot(x, y));
    }

    // Push to controllers
    ui_defs.push_back(controller);
  }

  // Close
  file.close();
  return 0;
}

// Get interfaces by ID
UI_Controller* InterfaceTypeManager::getInterfaceByID(int id) {
  if (id >= 0 && id < (signed)ui_defs.size())
    return ui_defs.at(id);
  return nullptr;
}
