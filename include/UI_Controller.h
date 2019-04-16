#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "Inventory.h"
#include "UI_Element.h"
#include "World.h"

class UI_Controller {
  public:
    UI_Controller(int width, int height);
    virtual ~UI_Controller();

    void Draw(BITMAP *buffer);

    void Update(World *wrld);

    void AddElement(UI_Element *element);

    Inventory *GetInventory();

  protected:

  private:
    UI_Element *ElementAt(int x, int y);

    bool is_open;
    Inventory *inv;
    int width, height;

    int x, y;

    std::vector<UI_Element*> elements;

    static ItemStack* mouse_item;

    int currently_bound;

};

#endif // UI_CONTROLLER_H
