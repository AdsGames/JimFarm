/*
  Interface Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of interfaces into a container for access by
  interface objects.
*/
#ifndef INTERFACE_TYPE_MANAGER_H
#define INTERFACE_TYPE_MANAGER_H

#define NON_SOLID 0
#define SOLID 1

#include <string>
#include <vector>

#include "../ui/UI_Controller.h"

class InterfaceTypeManager {
 public:
  InterfaceTypeManager(){};
  ~InterfaceTypeManager();

  // Load interface types
  static int loadInterfaces(std::string path);

  // Allows communication
  static UI_Controller* getInterfaceById(int id);

 private:
  // Stores all interfaces
  static std::vector<UI_Controller*> ui_defs;
};

#endif  // INTERFACE_TYPE_MANAGER_H
