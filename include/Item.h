#ifndef ITEM_H
#define ITEM_H

#include <string>

#include "manager/TileType.h"

#define MAX_ITEM_META 255

class Item {
  public:
    // Ctor and Dtor
    Item (char newID, unsigned char meta = 0);
    ~Item();

    // Draw to screen
    void draw (int x, int y, BITMAP *buffer);

    // Get value of item
    unsigned char getValue();

    // Modify ID
    unsigned char getID();

    // Get the name of item
    std::string getName();

    // Access and set meta data byte
    void setMeta (unsigned char meta) { this -> meta = meta; }
    void changeMeta (unsigned char amt) { this -> meta += amt; }
    unsigned char getMeta() { return this -> meta; }

  private:
    // Metadata info
    unsigned char meta;

    // Pointer to item type
    TileType *item_pointer;
};

#endif // ITEM_H
