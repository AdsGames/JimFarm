#ifndef TILE_UI_H
#define TILE_UI_H

#include "Inventory.h"
//#include "ui/UIHandler.h"

class Tile_UI {
  public:
    Tile_UI();
    virtual ~Tile_UI();

    open();

    close();

    toggle_ui();

    draw();

    update();

  protected:

  private:
    bool is_open;

    Inventory *inv;

    //UIHandler *ui;
};

#endif // TILE_UI_H
