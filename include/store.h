#ifndef STORE_H
#define STORE_H

#include <allegro.h>
#include <vector>

#include "character.h"
#include "inventory.h"

class store
{
  public:
    store();
    virtual ~store();

    bool open;

    inventory storeItems;

    void draw( BITMAP *tempBitmap);
    void draw_background( BITMAP *tempBitmap);

    void open_store( inventory *new_inventory);
    void close_store();

    void load_data();

    void add_item( item *storeItem){ storeItems.addItem( storeItem, 0);}

    void update();

  protected:

  private:
    int tick;

    BITMAP *image;
    BITMAP *image_background;
    BITMAP *indicator;

    SAMPLE *buy;
    SAMPLE *sell;
    SAMPLE *error;

    inventory *customer_inventory;

    int selector_index;
};

#endif // STORE_H
