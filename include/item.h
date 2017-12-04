#ifndef ITEM_H
#define ITEM_H

#include <allegro.h>
#include <string>

#include "tile_type.h"

class item
{
  public:
    item( char newID);
    virtual ~item();

    void draw( int newX, int newY, BITMAP *tempBuffer);

    tile_type *item_pointer;

    unsigned char getValue(){ return item_pointer -> getValue();}

    unsigned char getID(){ return item_pointer -> getID();}
    void setID( unsigned char newID);

    std::string getName(){ return item_pointer -> getName();}
  protected:
  private:
};

#endif // ITEM_H
