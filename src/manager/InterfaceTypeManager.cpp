#include "manager/InterfaceTypeManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "utility/Tools.h"

#include "UI_Label.h"
#include "UI_Slot.h"

std::vector<UI_Controller*> InterfaceTypeManager::ui_defs;


// Destructor
InterfaceTypeManager::~InterfaceTypeManager() {
  ui_defs.clear();;
}

// Load interfaces
int InterfaceTypeManager::load_interfaces(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path.c_str());
  if (!file)
    return 1;

  // Create buffer
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());

  // Get first node
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);
  rapidxml::xml_node<> *allInterfaces = doc.first_node();

  // Parse data
  for (rapidxml::xml_node<> *cInterface = allInterfaces -> first_node("interface");
       cInterface != nullptr;
       cInterface = cInterface -> next_sibling("interface")) {

    // Name of interface
    std::string name = cInterface-> first_node("name") -> value();

    // Width and Height
    int width = atoi(cInterface -> first_attribute("width") -> value());
    int height = atoi(cInterface -> first_attribute("height") -> value());

    // Create ui controller
    UI_Controller *controller = new UI_Controller(width, height);

    // Labels
    for (rapidxml::xml_node<> *cLabel = cInterface -> first_node("label"); cLabel != nullptr; cLabel = cLabel -> next_sibling("label")) {
      std::string text = cLabel-> first_attribute("text") -> value();
      int x = atoi(cLabel -> first_attribute("x") -> value());
      int y = atoi(cLabel -> first_attribute("y") -> value());
      controller -> AddElement(new UI_Label(x, y, text));
    }

    // Slots
    for (rapidxml::xml_node<> *cSlot = cInterface -> first_node("slot"); cSlot != nullptr; cSlot = cSlot -> next_sibling("slot")) {
      int x = atoi(cSlot -> first_attribute("x") -> value());
      int y = atoi(cSlot -> first_attribute("y") -> value());
      controller -> AddElement(new UI_Slot(x, y));
    }

    // Push to controllers
    ui_defs.push_back (controller);
  }

  // Close
  file.close();
  return 0;
}

// Get interfaces by ID
UI_Controller* InterfaceTypeManager::getInterfaceByID (int id) {
  if (id >= 0 && id < (signed)ui_defs.size())
    return ui_defs.at(id);
  return nullptr;
}
