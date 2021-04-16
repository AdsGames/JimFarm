/**
 * Key Listener
 *   Allan Legemaate
 * Listens for keys JUST pressed or JUST released
 *   since the last tick
 * 16/05/2016
 **/

#ifndef SRC_UTILITY_KEYLISTENER_H_
#define SRC_UTILITY_KEYLISTENER_H_

#define KL_KEY_MAX 127

class KeyListener {
 public:
  void update();

  static bool keyPressed[KL_KEY_MAX];
  static bool keyReleased[KL_KEY_MAX];
  static bool lastTicksKey[KL_KEY_MAX];

 protected:
 private:
};

#endif  // SRC_UTILITY_KEYLISTENER_H_
