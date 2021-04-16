/**
 * Mouse Listener
 * Allan Legemaate
 * Listens for mouse buttons JUST pressed or JUST released
 *   since the last tick
 * 18/01/2017
 **/

#include <allegro.h>

#ifndef SRC_UTILITY_MOUSELISTENER_H_
#define SRC_UTILITY_MOUSELISTENER_H_

#define MAX_MOUSE_BUTTONS 3

class MouseListener {
 public:
  void update();

  static unsigned char mouse_button;
  static unsigned char mouse_pressed;
  static unsigned char mouse_released;
  static int mouse_z_change;

 protected:
 private:
  static unsigned char mouse_old;
  static int mouse_z_old;
};

#endif  // SRC_UTILITY_MOUSELISTENER_H_
