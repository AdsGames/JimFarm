#ifndef HUD_H_
#define HUD_H_

#include <map>
#include <string>

#include "UiController.h"

class Hud {
 public:
  Hud();

  void draw();
  void update();

  void toggleUiController(const std::string& name,
                          const UiController& ui_controller);

  bool isOpen() const { return ui_controllers.size() > 0; }

 private:
  std::map<std::string, UiController> ui_controllers;
};

#endif  // HUD_H_