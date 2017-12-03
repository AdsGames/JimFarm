#include "game_menu.h"

#include "tools.h"
#include "keyListener.h"

game_menu::game_menu(){
  open = false;
}

game_menu::~game_menu(){}

void game_menu::load_data(){
  image_menu = load_bitmap_ex("images/game_menu.png");
}

void game_menu::trigger(){
  open = !open;
  indicator_position = 0;
}

void game_menu::update(){
  // Menu
  if( keyListener::keyPressed[KEY_ESC]){
    trigger();
  }

  // Change cursor location
  if( keyListener::keyPressed[KEY_DOWN]){
    indicator_position++;
    if( indicator_position > 2){
      indicator_position = 0;
    }
  }
  else if( keyListener::keyPressed[KEY_UP]){
    indicator_position--;
    if( indicator_position < 0){
      indicator_position = 2;
    }
  }

  // Select
  if( open && (key[KEY_SPACE] || key[KEY_ENTER])){
    // Menu
    if( indicator_position == 0){
      game_state = 0;
      open = false;
    }
    // Save
    else if( indicator_position == 1){

    }
    // Exit
    else{
      open = false;
    }
  }
}

// Draw menu
void game_menu::draw( BITMAP *tempImage){
  if( open){
    draw_sprite( tempImage, image_menu, 0, 0);
    rectfill( tempImage, 84, 58 + (indicator_position * 17), 84 + 9, 58 + (indicator_position * 17) + 9, makecol( 0, 0, 0));
    rectfill( tempImage, 136, 58 + (indicator_position * 17), 136 + 9, 58 + (indicator_position * 17) + 9, makecol( 0, 0, 0));
  }
}

// Return if opened
bool game_menu::isOpen(){
  return open;
}
