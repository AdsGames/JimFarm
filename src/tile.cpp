#include "tile.h"

// TILE
tile::tile(int newX, int newY, char newID)
{
    // Set init variables
    x = newX;
    y = newY;

    tile_pointer = tile_type_manager::getTileByID( newID);

    requirements_met = false;
}

tile::~tile()
{
    //dtor
}

void tile::draw( BITMAP *tempBuffer)
{
    tile_pointer -> draw( x, y, tempBuffer);
}

void tile::draw_at( int newX, int newY, BITMAP *tempBuffer)
{
    tile_pointer -> draw( newX, newY, tempBuffer);
}

void tile::setID( unsigned char newID){
  tile_pointer = tile_type_manager::getTileByID( newID);
}

void tile::run_tick(){
    /*// Berries
    if( tile_pointer -> getID() == 30 || tile_pointer -> getID() == 31 || tile_pointer -> getID() == 32){
        if( random( 0, 10) == 0){
            requirements_met = true;
        }
    }

    // Tomatoes
    else if( tile_pointer -> getID() == 33 || tile_pointer -> getID() == 34 || tile_pointer -> getID() == 35){
        if( random( 0, 30) == 0){
            requirements_met = true;
        }
    }

    // Carrot
    else if( tile_pointer -> getID() == 36 || tile_pointer -> getID() == 37 || tile_pointer -> getID() == 38){
        if( random( 0, 60) == 0){
            requirements_met = true;
        }
    }

    // Lavender
    else if( tile_pointer -> getID() == 39 || tile_pointer -> getID() == 40 || tile_pointer -> getID() == 41){
        if( random( 0, 100) == 0){
            requirements_met = true;
        }
    }*/

    // Plowed soil
    if( tile_pointer -> getID() == 2){
        if( random( 0, 400) == 0){
            requirements_met = true;
        }
    }
    else if( tile_pointer -> getID() == 18){
        if( random( 0, 400) == 0){
            requirements_met = true;
        }
    }
}


// ITEM
item::item(int newX, int newY, char newID) : tile( newX, newY, newID)
{
    tile_pointer = tile_type_manager::getItemByID( newID);
}

item::~item()
{
    //dtor
}

void item::draw( BITMAP *tempBuffer)
{
    tile_pointer -> draw( x, y, tempBuffer);
}
