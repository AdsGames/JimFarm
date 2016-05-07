#include "character.h"

character::character()
{
}

character::~character()
{
    //dtor
}

void character::draw( BITMAP *tempBuffer)
{
    draw_sprite( tempBuffer, image, x, y - 3);
}

void character::update(){
    // Move
    if( direction == 0){
        if( key[KEY_UP]){
            direction = 1;
        }
        else if( key[KEY_DOWN]){
            direction = 2;
        }
        else if( key[KEY_LEFT]){
            direction = 3;
        }
        else if( key[KEY_RIGHT]){
            direction = 4;
        }

        gameTick = 0;

        // Action button
        if( key[KEY_SPACE]){
            map_pointer -> replace_tile( x, y, 2);
        }
    }

    // Smooth move
    if( direction == 1){
        if( y > 0)
            y -= 2;
    }
    else if( direction == 2){
        if( y < 160 - 16)
            y += 2;
    }
    else if( direction == 3){
        if( x > 0)
            x -= 2;
    }
    else if( direction == 4){
        if( x < 240 - 16)
            x += 2;
    }

    // Snap
    if( x % 16 == 0 && y % 16 == 0 ){
        direction = 0;
    }

    // Increase game ticker
    gameTick++;
    if( gameTick > 8)
        gameTick = 0;
}

void character::setWorld( tile_map *newTileMap){
    map_pointer = newTileMap;
}
