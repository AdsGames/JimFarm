#include "character.h"

character::character()
{
    moving = false;
    direction = 1;
}

character::~character()
{
    //dtor
}

// Set image
void character::setImage( BITMAP *newImage){

    image = newImage;

    // Load fonts
    f1 = load_font("fonts/pixelart.pcx", NULL, NULL);
    f2 = extract_font_range(f1, ' ', 'A'-1);
    f3 = extract_font_range(f1, 'A', 'Z');
    f4 = extract_font_range(f1, 'Z'+1, 'z');
    pixelart = merge_fonts(f4, f5 = merge_fonts(f2, f3));

    // Destroy temporary fonts
    destroy_font(f1);
    destroy_font(f2);
    destroy_font(f3);
    destroy_font(f4);
    destroy_font(f5);
}

// Draw character to screen
void character::draw( BITMAP *tempBuffer)
{



  masked_blit( image, tempBuffer, floor(gameTick/2) * 16, (direction - 1) * 20, x, y - 3, 16, 20);

  textprintf_ex(tempBuffer,pixelart,5,0,makecol(255,255,255),-1,"%i",water);
  textprintf_ex(tempBuffer,pixelart,20,0,makecol(255,255,255),-1,"Water");
  textprintf_ex(tempBuffer,pixelart,5,10,makecol(255,255,255),-1,"%i",inventory);
  textprintf_ex(tempBuffer,pixelart,20,10,makecol(255,255,255),-1,"Item");
  textprintf_ex(tempBuffer,pixelart,5,20,makecol(255,255,255),-1,"%i",money);
  textprintf_ex(tempBuffer,pixelart,20,20,makecol(255,255,255),-1,"Ca$hMoney$");
}

void character::update(){
    // Ask joystick for keys
    poll_joystick();

    //Oh
    // Snap
    if( x % 16 == 0 && y % 16 == 0 ){
        gameTick = 0;
        moving = false;
    }

    // Move
    if( !moving){
        if( key[KEY_UP]){
            direction = 2;
            moving = true;
        }
        else if( key[KEY_DOWN]){
            direction = 1;
            moving = true;
        }
        else if( key[KEY_LEFT]){
            direction = 4;
            moving = true;
        }
        else if( key[KEY_RIGHT]){
            direction = 3;
            moving = true;
        }

        // Action button
        if( key[KEY_SPACE] || joy[0].button[0].b){
            if( map_pointer -> get_tile_at( x, y) == 0)
                map_pointer -> replace_tile( x, y, 2);
        }
    }

    if( moving){
        // Smooth move
        if( direction == 2){
            if( y > 0)
                y -= 2;
        }
        else if( direction == 1){
            if( y < 160 - 16)
                y += 2;
        }
        else if( direction == 4){
            if( x > 0)
                x -= 2;
        }
        else if( direction == 3){
            if( x < 240 - 16)
                x += 2;
        }

        // Increase game ticker
        gameTick++;
        if( gameTick > 7)
            gameTick = 0;
    }
}

void character::setWorld( tile_map *newTileMap){
    map_pointer = newTileMap;
}
