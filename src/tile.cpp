#include "tile.h"

#include <iostream>
#include <math.h>

#include "tools.h"
#include "tile_type_manager.h"
#include "tile_defs.h"
#include "item_defs.h"

// TILE
tile::tile(int newX, int newY, char newID){
  // Set init variables
  x = newX;
  y = newY;

  tile_pointer = tile_type_manager::getTileByID( newID);

  requirements_met = false;
}

tile::~tile(){}

void tile::draw( BITMAP *tempBuffer){
  tile_pointer -> draw( x, y, tempBuffer);
  if( requirements_met)
    rect( tempBuffer, x, y + 16, x + getImgWidth() * 16, (y + 16) - (ceil(double(getImgHeight())/2) * 16), 0xFFFF00);
}

void tile::draw_at( int newX, int newY, BITMAP *tempBuffer){
  tile_pointer -> draw( newX, newY, tempBuffer);
}

void tile::setID( unsigned char newID){
  tile_pointer = tile_type_manager::getTileByID( newID);
}

void tile::run_tick(){
  // Berries
  if( tile_pointer -> getID() == TILE_BERRY_1 || tile_pointer -> getID() == TILE_BERRY_2 || tile_pointer -> getID() == TILE_BERRY_3){
    if( random( 0, 10) == 0){
      requirements_met = true;
    }
  }

  // Tomatoes
  else if( tile_pointer -> getID() == TILE_TOMATO_1 || tile_pointer -> getID() == TILE_TOMATO_2 || tile_pointer -> getID() == TILE_TOMATO_3){
    if( random( 0, 30) == 0){
      requirements_met = true;
    }
  }

  // Carrot
  else if( tile_pointer -> getID() == TILE_CARROT_1 || tile_pointer -> getID() == TILE_CARROT_2 || tile_pointer -> getID() == TILE_CARROT_3){
    if( random( 0, 60) == 0){
      requirements_met = true;
    }
  }

  // Lavender
  else if( tile_pointer -> getID() == TILE_LAVENDER_1 || tile_pointer -> getID() == TILE_LAVENDER_2 || tile_pointer -> getID() == TILE_LAVENDER_3){
    if( random( 0, 100) == 0){
      requirements_met = true;
    }
  }

  // Plowed soil
  if( tile_pointer -> getID() == TILE_SOIL){
    if( random( 0, 400) == 0){
      requirements_met = true;
    }
  }
  else if( tile_pointer -> getID() == TILE_PLOWED_SOIL){
    if( random( 0, 400) == 0){
      requirements_met = true;
    }
  }
}
