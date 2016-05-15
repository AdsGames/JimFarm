#include "menu.h"


menu::menu()
{
    state = MAIN_MENU;
}

menu::~menu()
{
    //dtor
}

void menu::load_data(){
    if (!( menu_image = load_bitmap("images/title_screen.png", NULL)))
        abort_on_error("Cannot find image images/title_screen.png\nPlease check your files and try again");

    if (!( help_image = load_bitmap("images/help.png", NULL)))
        abort_on_error("Cannot find image images/help.png\nPlease check your files and try again");

    if (!( story_image = load_bitmap("images/story.png", NULL)))
        abort_on_error("Cannot find image images/story.png\nPlease check your files and try again");

    if (!( coin_flip = load_bitmap("images/coin_flip.png", NULL)))
        abort_on_error("Cannot find image coin_flip.png\nPlease check your files and try again");


}


void menu::draw( BITMAP *tempBitmap){
    if(state==MAIN_MENU){
        draw_sprite( tempBitmap, menu_image, 0, 0);
        masked_blit( coin_flip, tempBitmap,9*(coin_frame/5),0,190,147-(indicator_location*14),9,9);
    }

}

bool menu::update(){


    tick++;
    if((key[KEY_SPACE] || key[KEY_LCONTROL]) && tick>10 && indicator_location == 3)
      return true;

    if(key[KEY_DOWN] && tick>10){
      tick=0;
      indicator_location--;
    }
    if(key[KEY_UP] && tick>10){
      tick=0;
      indicator_location++;
    }
    if(indicator_location>3){
      indicator_location=0;
    }
    if(indicator_location<0){
      indicator_location=3;
    }

    // Coin spin
    if(!coin_direction)
      coin_frame++;
    if(coin_direction)
      coin_frame--;
    if(coin_frame>18)
      coin_direction=true;
    if(coin_frame<0){
      coin_frame=5;
      coin_direction=false;
    }

    if(!((key[KEY_SPACE] || key[KEY_LCONTROL]) && tick>10 && indicator_location == 3))
      return false;


}
