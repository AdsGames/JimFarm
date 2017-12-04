#include "item.h"

#include "tile_type_manager.h"

// ITEM
item::item( char newID){
  item_pointer = tile_type_manager::getItemByID( newID);
}

item::~item(){
  //dtor
}

void item::draw( int newX, int newY, BITMAP *tempBuffer){
  item_pointer -> draw( newX, newY, tempBuffer);
}

void item::setID( unsigned char newID){
  item_pointer = tile_type_manager::getTileByID( newID);
}
