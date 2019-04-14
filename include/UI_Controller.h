#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "Inventory.h"
#include "UI_Element.h"
#include "World.h"

class UI_Controller {
  public:
    UI_Controller(int size, int width, int height);
    virtual ~UI_Controller();

    void Draw(BITMAP *buffer);

    void Update(World *wrld);

    void AddElement(UI_Element *element);

  protected:

  private:
    UI_Element *ElementAt(int x, int y);

    bool is_open;
    Inventory inv;
    int width, height;

    int x, y;

    std::vector<UI_Element*> elements;

    FONT *ui_font;

    static Item* mouse_item;

};

#endif // UI_CONTROLLER_H
