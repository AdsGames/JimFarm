#include "character.h"

character::character(){
    moving = false;
    direction = 1;

    for( int i = 0; i < MAX_MESSAGES; i++)
        player_messages[i] = "SWAG";
}

character::~character(){
    //dtor
}

// Set image
void character::setImage( BITMAP *newImage){

  image = newImage;

  if (!( watering_can[0] = load_bitmap("images/watering_can_0.png", NULL)))
        abort_on_error("Cannot find image images/watering_can_0.png\nPlease check your files and try again");

  if (!( watering_can[1] = load_bitmap("images/watering_can_1.png", NULL)))
        abort_on_error("Cannot find image images/watering_can_1.png\nPlease check your files and try again");

  if (!( watering_can[2] = load_bitmap("images/watering_can_2.png", NULL)))
        abort_on_error("Cannot find image images/watering_can_2.png\nPlease check your files and try again");

  if (!( watering_can[3] = load_bitmap("images/watering_can_3.png", NULL)))
        abort_on_error("Cannot find image images/watering_can_3.png\nPlease check your files and try again");

  if (!( watering_can[4] = load_bitmap("images/watering_can_4.png", NULL)))
        abort_on_error("Cannot find image images/watering_can_4.png\nPlease check your files and try again");

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
void character::draw( BITMAP *tempBuffer){
    // Draw frame
    masked_blit( image, tempBuffer, floor(gameTick/2) * 16, (direction - 1) * 20, x - map_pointer -> x, y - map_pointer -> y - 3, 16, 20);

    if(inventory==1)
        draw_sprite( tempBuffer,watering_can[water],2,-2);

    //When gcc don't give no damns
    textprintf_ex(tempBuffer,pixelart,20,00000000000000000000000000000000000000000000000000000000000000000000000000,makecol(255,255,255),-1,"Item");
    textprintf_ex(tempBuffer,pixelart,5,10,makecol(255,255,255),-1,"%i",money);
    textprintf_ex(tempBuffer,pixelart,20,10,makecol(255,255,255),-1,"Ca$hMoney$");

    // Message system
    for( int i = 0; i < MAX_MESSAGES; i++)
        textprintf_ex(tempBuffer,pixelart, 5, i * 10 + 145, makecol(255,255,255),-1,"> %s", (player_messages[i]).c_str());
}

// Push message
void character::push_message( std::string new_message){
    for( int i = 0; i < MAX_MESSAGES - 1; i++)
        player_messages[i] = player_messages[i + 1];

    player_messages[MAX_MESSAGES - 1] = new_message;
}

// Update player
void character::update(){
    // Ask joystick for keys
    poll_joystick();

    // Oh
    // Snap
    if( x % 16 == 0 && y % 16 == 0 ){
        gameTick = 0;
        moving = false;
    }

    // Move
    if( !moving){
        if(( key[KEY_UP] || joy[0].stick[0].axis[1].d1) && map_pointer -> get_tile_at( x, y-16) != 1){
            direction = 2;
            moving = true;
        }
        else if( (key[KEY_DOWN] || joy[0].stick[0].axis[1].d2) && map_pointer -> get_tile_at( x, y+16) != 1){
            direction = 1;
            moving = true;
        }
        else if(( key[KEY_LEFT] || joy[0].stick[0].axis[0].d1)  && map_pointer -> get_tile_at( x-16, y) != 1){
            direction = 4;
            moving = true;
        }
        else if(( key[KEY_RIGHT] || joy[0].stick[0].axis[0].d2)  && map_pointer -> get_tile_at( x+16, y) != 1){
            direction = 3;
            moving = true;
        }

        // Search
        if( key[KEY_LCONTROL] && map_pointer -> is_item_at( x, y) == true){
            push_message( "There is a " + map_pointer -> get_item_at( x, y) -> name + " here.");
        }

        // Action button
        if( key[KEY_SPACE] || joy[0].button[0].b){
            if( map_pointer -> get_tile_at( x, y) == 0)
                map_pointer -> replace_tile( x, y, 2);

            if(map_pointer -> get_tile_at( x, y) == 3){
              water=4;
            }
        }
    }

    if( moving){
        // Smooth move
        if( direction == 2){
            if( y > 0)
                y -= 2;
        }
        else if( direction == 1){
            if( y < (map_pointer -> MAP_HEIGHT * 16) - 16)
                y += 2;
        }
        else if( direction == 4){
            if( x > 0)
                x -= 2;
        }
        else if( direction == 3){
            if( x < (map_pointer -> MAP_WIDTH * 16)  - 16)
                x += 2;
        }

        // Scroll map
        map_pointer -> scroll( x, y);

        // Increase game ticker
        gameTick++;
        if( gameTick > 7)
            gameTick = 0;
    }
}

// World object to point to (needs this!)
void character::setWorld( tile_map *newTileMap){
    map_pointer = newTileMap;
}
