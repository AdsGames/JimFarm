/*
  Interface Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of interfaces into a container for access by
  interface objects.
*/
#ifndef INTERFACE_TYPE_MANAGER_H
#define INTERFACE_TYPE_MANAGER_H

#include <string>
#include <vector>

#include "../ui/UI_Controller.h"

class InterfaceTypeManager {
 public:
  // Load interface types
  static int loadInterfaces(const std::string& path);

  // Allows communication
  static UI_Controller& getInterfaceById(int id);

 private:
  // Stores all interfaces
  static std::vector<UI_Controller> ui_defs;
};

#endif  // INTERFACE_TYPE_MANAGER_H
