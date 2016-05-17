#include<allegro.h>
#include<alpng.h>

#include "tile.h"
#include "character.h"
#include "tile_map.h"
#include "tools.h"
#include "store.h"
#include "menu.h"
#include "keyListener.h"

#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"


// Images
BITMAP *buffer;
BITMAP *jim_image;

// Music
FSOUND_STREAM* music;

bool close_button_pressed;

// Listen to the keys
keyListener keys;

#define MENU 0
#define GAME 1

int game_state = 0;

tile_map farm_map;
character jim;
menu game_menu;

store main_store;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;

void ticker(){
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker(){
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void){
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

// Game clock
int animationFrame = 0;

void animationTicker(){
    animationFrame++;
    if( animationFrame > 10)
        animationFrame = 0;
}
END_OF_FUNCTION(ticker)


/*********************
 *   Update logic
 *********************/
void update(){
    // Checks keys JUST pressed or JUST released
    keys.update();

    if( game_state == MENU){
        // HAXX
        // halp plz how do i coed?
        int menu_result = game_menu.update();
        if(menu_result==2){
          game_state=GAME;
        }
        if(menu_result==0){
          close_button_pressed=true;
        }
    }
    else if( game_state == GAME){
        // Update character
        farm_map.update();
        jim.update();

        // Store update
        main_store.update();


        // Open store
        if( jim.store_open){
            main_store.open_store( &jim);
        }
        else{
            main_store.close_store();
        }
    }
}

/*********************
 *  Draw to screen
 *********************/
void draw(){
    if( game_state == MENU){
        game_menu.draw(buffer);

        // Stretch screen
        stretch_sprite( screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
    }
    else if( game_state == GAME){
        // Draw background
        rectfill( buffer, 0, 0, 160, 240, makecol( 0, 0, 0));

        // Draw map
        farm_map.draw( buffer);

        // Draw JIM
        jim.draw( buffer);

        // Draw map
        farm_map.drawForeground( buffer);


        // Allan says this is bad code.
        // I disagree halfheartedly.

        main_store.draw_background(buffer);
        // Draw JIM
        jim.drawForeground( buffer);

        main_store.draw( buffer);

        // Stretch screen
        stretch_sprite( screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
    }
}


/*********************
 *   Setup game
 *********************/
void setup(){

    // Create buffer
    buffer = create_bitmap( 240, 160);

    srand(time(NULL));

    // Music
    music = FSOUND_Stream_Open( "sfx/farmy.mp3", 2, 0, 0);
    FSOUND_Stream_Play( 0, music);
    FSOUND_SetVolumeAbsolute( 0, 0);

     // Setup for FPS system
    LOCK_VARIABLE(ticks);
    LOCK_FUNCTION(ticker);
    install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(game_time_ticker);
    install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

    // Animation ticks
    LOCK_VARIABLE(animationFrame);
    LOCK_FUNCTION(animationTicker);
    install_int( animationTicker,10);
    install_int_ex( animationTicker, BPS_TO_TIMER(100));


    // Close button
    LOCK_FUNCTION(close_button_handler);
    set_close_button_callback(close_button_handler);

    // Nice Map
    farm_map.load_images();
    farm_map.generate_map();

    // Main menu loading
    game_menu.load_data();

    // Setup jim
    jim.setPosition( 15 * 16, 5 * 16);

    jim.load_data();
    jim.setWorld( &farm_map);

    // Store
    main_store.load_data();
}


/*********************
 *   Start here
 *********************/
int main(){

  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  install_joystick(JOY_TYPE_AUTODETECT);
  FSOUND_Init (44100, 32, 0);

  set_color_depth(32);

  int w, h;
  get_desktop_resolution(&w, &h);

  //set_gfx_mode(GFX_AUTODETECT, w,h, 0, 0);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1280,960, 0, 0);

  //set_gfx_mode(GFX_AUTODETECT_WINDOWED, 240,160, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");



  set_window_title("Jim Farm");
  setup();


    while(!close_button_pressed){
        if(key[KEY_ESC] && !jim.store_open)
          break;
        while(ticks == 0){
            rest(1);
        }
        while(ticks > 0){
            int old_ticks = ticks;

            update();

            ticks--;
            if(old_ticks <= ticks){
                break;
            }
        }
        if(game_time - old_time >= 10){
            fps = frames_done;
            frames_done = 0;
            old_time = game_time;
        }
        draw();
    }


	return 0;
}
END_OF_MAIN()
