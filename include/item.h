#ifndef ITEM_H
#define ITEM_H

#include <allegro.h>
#include <string>

#include "tile_type.h"

#define MAX_ITEM_META 255

class item
{
  public:
    // Ctor and Dtor
    item (char newID, unsigned char meta = 0);
    ~item();

    // Draw to screen
    void draw (int newX, int newY, BITMAP *tempBuffer);

    // Get value of item
    unsigned char getValue() { return item_pointer -> getValue();}

    // Modify ID
    unsigned char getID() { return item_pointer -> getID();}

    // Get the name of item
    std::string getName() { return item_pointer -> getName();}

    // Access and set meta data byte
    void setMeta (unsigned char meta ) { this -> meta = meta; }
    void changeMeta (unsigned char amt ) { this -> meta += amt; }
    unsigned char getMeta() { return this -> meta; }
  protected:
  private:
    // Metadata info
    unsigned char meta;

    // Pointer to item type
    tile_type *item_pointer;
};

#endif // ITEM_H
