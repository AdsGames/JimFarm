#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<vector>

#include "tile.h"
#include "character.h"

// Images
BITMAP *buffer;
BITMAP *tile_images[100];
BITMAP *jim_image;

bool close_button_pressed;

// Tiles
std::vector<tile> map_tiles;

character jim;

// Constant
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 10;


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


// Random number generator. Use int random(highest,lowest);
int random(int newLowest, int newHighest)
{
  int lowest = newLowest, highest = newHighest;
  int range = (highest - lowest) + 1;
  int randomNumber = lowest+int(range*rand()/(RAND_MAX + 1.0));
  return randomNumber;
}


//A function to streamline error reporting in file loading
void abort_on_error(const char *message){
	 set_window_title("Error!");
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}

/*********************
 *   Update logic
 *********************/
void update(){
    // Update character
    jim.update();
}

/*********************
 *  Draw to screen
 *********************/
void draw(){
    // Draw background
    rectfill( buffer, 0, 0, 160, 240, makecol( 0, 0, 0));

    // Draw tiles
    for( int i = 0; i < map_tiles.size(); i++){
        map_tiles.at(i).draw( buffer);
    }

    // Draw JIM
    jim.draw( buffer);

    // Stretch screen
    stretch_sprite( screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
}


/*********************
 *   Setup game
 *********************/
void setup(){

    // Create buffer
    buffer = create_bitmap( 240, 160);

    srand(time(NULL));

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

    // Check if image exists
    if (!( tile_images[0] = load_bitmap("images/grass.png", NULL)))
        abort_on_error("Cannot find image images/grass.png\nPlease check your files and try again");

    if (!( tile_images[1] = load_bitmap("images/water.png", NULL)))
        abort_on_error("Cannot find image images/water.png\nPlease check your files and try again");

    if (!( jim_image = load_bitmap("images/tile2.png", NULL)))
        abort_on_error("Cannot find image images/tile2.png\nPlease check your files and try again");

    if (!( tile_images[2] = load_bitmap("images/tile1.png", NULL)))
        abort_on_error("Cannot find image images/tile2.png\nPlease check your files and try again");

    // Nice Map
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            if( i > 6){
                tile newTile( i * 16, t * 16, tile_images[1], tile_images[1], 1);
                map_tiles.push_back( newTile);
            }
            else{
                tile newTile( i * 16, t * 16, tile_images[0], tile_images[0], 0);
                map_tiles.push_back( newTile);
            }
        }
    }

    // Setup jim
    jim.setPosition( 0, 0);
    jim.setImage( jim_image);
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
  set_color_depth(32);


  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 960, 640, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");



  set_window_title("Tojam \"Don't stop Allan'snoseing\"");
  setup();


    while(!key[KEY_ESC] && !close_button_pressed){
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
