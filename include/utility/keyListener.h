/**
 * Key Listener
 *   Allan Legemaate
 * Listens for keys JUST pressed or JUST released
 *   since the last tick
 * 16/05/2016
**/

#ifndef KeyListener_H
#define KeyListener_H

#define KL_KEY_MAX 127

class KeyListener {
  public:
    KeyListener() {};
    virtual ~KeyListener() {};

    void update();

    static bool keyPressed[KL_KEY_MAX];
    static bool keyReleased[KL_KEY_MAX];
    static bool lastTicksKey[KL_KEY_MAX];
  protected:
  private:
};

#endif // KeyListener_H
