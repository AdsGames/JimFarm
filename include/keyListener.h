/**
 * Key Listener
 *   Allan Legemaate
 * Listens for keys JUST pressed or JUST released
 *   since the last tick
 * 16/05/2016
**/

#ifndef KEYLISTENER_H
#define KEYLISTENER_H

#include <allegro.h>

class keyListener
{
  public:
    keyListener();
    virtual ~keyListener();

    void update();

    static bool keyPressed[KEY_MAX];
    static bool keyReleased[KEY_MAX];

  protected:
  private:
    static bool lastTicksKey[KEY_MAX];
};

#endif // KEYLISTENER_H
