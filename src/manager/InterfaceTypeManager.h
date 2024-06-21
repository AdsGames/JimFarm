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

#include "../ui/UiController.h"

class InterfaceTypeManager {
 public:
  // Load interface types
  static int loadInterfaces(const std::string& path);

  // Allows communication
  static UiController& getInterfaceById(int id);

  // Get interface by name
  static UiController& getInterfaceByName(const std::string& name);

 private:
  // Stores all interfaces
  static std::vector<UiController> ui_defs;
};

#endif  // INTERFACE_TYPE_MANAGER_H
