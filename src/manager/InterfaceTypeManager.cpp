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
  for (unsigned int i = 0; i < doc.size(); i++) {
    // Name of interface
    std::string name = doc[i]["name"];

    // Width and Height
    int width = doc[i]["width"];
    int height = doc[i]["height"];

    // Create ui controller
    UI_Controller* controller = new UI_Controller(width, height);

    // Labels
    for (unsigned int t = 0; t < doc[i]["labels"].size(); t++) {
      std::string text = doc[i]["labels"][t]["text"];
      int x = doc[i]["labels"][t]["x"];
      int y = doc[i]["labels"][t]["y"];
      controller->AddElement(new UI_Label(x, y, text));
    }

    // Slots
    for (unsigned int t = 0; t < doc[i]["slots"].size(); t++) {
      int x = doc[i]["slots"][t]["x"];
      int y = doc[i]["slots"][t]["y"];
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
