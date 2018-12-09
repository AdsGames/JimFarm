#include "KeyListener.h"

#include <allegro.h>

bool keyListener::keyPressed[KL_KEY_MAX] = { false};
bool keyListener::keyReleased[KL_KEY_MAX] = { false};
bool keyListener::lastTicksKey[KL_KEY_MAX] = { false};

// Check those keys!
void keyListener::update() {
  // Check key just pressed
  for (int i = 0; i < KL_KEY_MAX; i++) {
    // Clear old values
    keyPressed[i] = false;
    keyReleased[i] = false;

    // Pressed since last tick?
    if ((bool)key[i] == true && lastTicksKey[i] == false) {
      keyPressed[i] = true;
      // std::cout << "Key: " << i << " pressed. \n";
    }

    // Released since last tick?
    if ((bool)key[i] == false && lastTicksKey[i] == true) {
      keyReleased[i] = true;
      // std::cout << "Key: " << i << " released. \n";
    }
  }


  // Get new values
  for (int i = 0; i < KL_KEY_MAX; i++) {
    // Key changed
    if (lastTicksKey[i] != (bool)key[i]) {
      //std::cout << "Key: " << i << " was " << lastTicksKey[i] << " and became " << (bool)key[i] << "\n";
      lastTicksKey[i] = key[i];
    }
  }
}
