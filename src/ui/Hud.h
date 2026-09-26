#ifndef HUD_H_
#define HUD_H_

#include <map>
#include <string>

#include "UiController.h"

class Hud {
 public:
  Hud();

  void draw(const GameState& state);
  void update(GameState& state);

  void toggleUiController(const std::string& name,
                          const UiController& ui_controller);

  // Open a window from interfaces.json by name, if not already open.
  // Vertical position as a fraction of the free screen height, -1 centres.
  void open(const std::string& name, float vertical = -1.0F);

  // Close every window
  void closeAll();

  bool isOpen() const { return ui_controllers.size() > 0; }

  // Ignore input, e.g. while the recipe book is on top
  void setBlocked(bool value) { blocked = value; }
  bool isOpen(const std::string& name) const {
    return ui_controllers.contains(name);
  }

 private:
  // Return items held on the mouse once no windows are open
  void returnMouseItem();

  std::map<std::string, UiController> ui_controllers;

  bool blocked{false};
};

#endif  // HUD_H_
