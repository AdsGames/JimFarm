#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <allegro.h>

class GameMenu {
  public:
    GameMenu();
    virtual ~GameMenu();

    void load_data();
    void trigger();
    void update();
    void draw (BITMAP *tempImage);
    bool isOpen();
  protected:
  private:
    BITMAP *image_menu;

    bool open;

    int indicator_position;
};

#endif // GAME_MENU_H
