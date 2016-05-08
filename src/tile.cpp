#include "tile.h"

// TILE
tile::tile(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID)
{
    // Set init variables
    x = newX;
    y = newY;

    image[0] = newImage1;
    image[1] = newImage2;

    id = newID;

    requirements_met = false;

    if( id == 1 || id == 3 || id == 5 || id == 50 || id == 6 || id == 11 ||id == 50 || id == 51 || id == 52 || id == 53 || id == 55 || id == 56 || id == 57 || id == 58 || id ==60 || id == 61 || id == 99)
        solid = true;
    else
        solid = false;
}

tile::~tile()
{
    //dtor
}

void tile::draw( BITMAP *tempBuffer)
{
    draw_sprite( tempBuffer, image[0], x, y - (image[0] -> h - 16));
}

void tile::run_tick(){
    // Berries
    if( id == 30 || id == 31 || id == 32){
        if( random( 0, 10) == 0){
            requirements_met = true;
        }
    }

    // Tomatoes
    else if( id == 33 || id == 34 || id == 35){
        if( random( 0, 30) == 0){
            requirements_met = true;
        }
    }

    // Carrot
    else if( id == 36 || id == 37 || id == 38){
        if( random( 0, 60) == 0){
            requirements_met = true;
        }
    }

    // Lavender
    else if( id == 39 || id == 40 || id == 41){
        if( random( 0, 100) == 0){
            requirements_met = true;
        }
    }

    else if( id == 18){
        if( random( 0, 400) == 0){
            requirements_met = true;
        }
    }
}


// ITEM
item::item(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, std::string newName) :
    tile( newX, newY, newImage1, newImage2, newID)
{
    // Set init variables
    x = newX;
    y = newY;

    image[0] = newImage1;
    image[1] = newImage2;

    id = newID;

    name = newName;

    switch ( id){
        case 0:
            value = 10;
            break;
        case 1:
            value = 20;
            break;
        case 2:
            value = 1;
            break;
        case 3:
            value = 20;
            break;
        case 4:
            value = 20;
            break;
        case 5:
            value = 20;
            break;
        case 6:
            value = 30;
            break;
        case 7:
            value = 1;
            break;
        default:
            value = 0;
    }
}

item::~item()
{
    //dtor
}





// CROP
crop::crop(int newX, int newY, BITMAP *newImage1, BITMAP *newImage2, char newID, std::string newName) :
    tile( newX, newY, newImage1, newImage2, newID)
{
    // Set init variables
    x = newX;
    y = newY;

    image[0] = newImage1;
    image[1] = newImage2;

    id = newID;

    name = newName;
}

crop::~crop()
{
    //dtor
}

void crop::run_tick() {

}

