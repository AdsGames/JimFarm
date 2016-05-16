#include "menu.h"


menu::menu()
{
    state = MAIN_MENU;
    indicator_location = 4;
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

    if (!( options_image = load_bitmap("images/options.png", NULL)))
        abort_on_error("Cannot find image images/options.png\nPlease check your files and try again");

    if (!( coin_flip = load_bitmap("images/coin_flip.png", NULL)))
        abort_on_error("Cannot find image coin_flip.png\nPlease check your files and try again");

     if( !(blip = load_sample("sfx/blip.wav")))
        abort_on_error( "Cannot find file sfx/blip.wav \n Please check your files and try again");


}


void menu::draw( BITMAP *tempBitmap){

    draw_sprite( tempBitmap, menu_image, 0, 0);

    if(state==MAIN_MENU){
        masked_blit( coin_flip, tempBitmap,9*(coin_frame/5),0,187,147-(indicator_location*11),9,9);
    }
    if(state==HELP){
      draw_sprite(tempBitmap,help_image,0,0);
    }
    if(state==STORY){
      draw_sprite(tempBitmap,story_image,0,0);
    }

    if(state==OPTIONS){
      draw_sprite(tempBitmap,options_image,0,0);
    }

}

int menu::update(){


    tick++;
    if(state==MAIN_MENU){
      if((key[KEY_SPACE] || key[KEY_LCONTROL]) && tick>10){
        tick=0;
        if(indicator_location==4){
          play_sample(blip,255,125,1000,0);
          return 2;
        }
        if(indicator_location==3){
          play_sample(blip,255,125,1000,0);
          state=OPTIONS;
        }
        if(indicator_location==2){
          state=HELP;
          play_sample(blip,255,125,1000,0);
        }
        if(indicator_location==1){
          state=STORY;
          play_sample(blip,255,125,1000,0);
        }
        if(indicator_location==0)
          return 0;
      }



      if(key[KEY_DOWN] && tick>10){
        play_sample(blip,255,125,1000,0);
        tick=0;
        indicator_location--;
      }
      if(key[KEY_UP] && tick>10){
        play_sample(blip,255,125,1000,0);
        tick=0;
        indicator_location++;
      }
      if(indicator_location>4){
        indicator_location=0;
      }
      if(indicator_location<0){
        indicator_location=4;
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
    }
    if((state==HELP || state==STORY || state==OPTIONS) &&(key[KEY_SPACE] || key[KEY_LCONTROL] || key[KEY_UP] || key[KEY_DOWN] || key[KEY_LEFT] || key[KEY_RIGHT])&& tick>10){
      play_sample(blip,255,125,1000,0);
      tick=0;
      state=MAIN_MENU;

    }


    return 1;


}
