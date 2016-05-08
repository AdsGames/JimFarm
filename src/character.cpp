#include "character.h"

character::character(){
    moving = false;
    direction = 1;

    for( int i = 0; i < MAX_MESSAGES; i++)
        player_messages[i] = "SWAG";

    money = 2;

    store_open = false;
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

    if (!( inventory_gui = load_bitmap("images/GUI_INVENTORY.png", NULL)))
        abort_on_error("Cannot find image images/GUI_INVENTORY.png\nPlease check your files and try again");

    if (!( hand = load_bitmap("images/hand.png", NULL)))
        abort_on_error("Cannot find image images/hand.png\nPlease check your files and try again");

    if (!( indicator = load_bitmap("images/indicator.png", NULL)))
        abort_on_error("Cannot find image images/indicator.png\nPlease check your files and try again");

    if (!( coin = load_bitmap("images/coin.png", NULL)))
        abort_on_error("Cannot find image images/coin.png\nPlease check your files and try again");

    if( !(pickup = load_sample("sfx/pickup.wav")))
        abort_on_error( "Cannot find file sfx/pickup.wav \n Please check your files and try again");

    if( !(drop = load_sample("sfx/drop.wav")))
        abort_on_error( "Cannot find file sfx/drop.wav \n Please check your files and try again");

    if( !(step_1 = load_sample("sfx/step_1.wav")))
        abort_on_error( "Cannot find file sfx/step_1.wav \n Please check your files and try again");

    if( !(step_2 = load_sample("sfx/step_2.wav")))
        abort_on_error( "Cannot find file sfx/step_2.wav \n Please check your files and try again");

    if( !(cut_axe= load_sample("sfx/cut_axe.wav")))
        abort_on_error( "Cannot find file sfx/cut_axe.wav \n Please check your files and try again");

    if( !(cut_scythe= load_sample("sfx/cut_scythe.wav")))
        abort_on_error( "Cannot find file sfx/cut_scythe.wav \n Please check your files and try again");

    if( !(hoe= load_sample("sfx/hoe.wav")))
        abort_on_error( "Cannot find file sfx/hoe.wav \n Please check your files and try again");

    if( !(error= load_sample("sfx/error.wav")))
        abort_on_error( "Cannot find file sfx/error.wav \n Please check your files and try again");

    if( !(water_fill= load_sample("sfx/water_fill.wav")))
        abort_on_error( "Cannot find file sfx/water_fill.wav \n Please check your files and try again");

    if( !(water_pour = load_sample("sfx/water_pour.wav")))
        abort_on_error( "Cannot find file sfx/water_pour.wav \n Please check your files and try again");

    if( !(dig = load_sample("sfx/dig.wav")))
        abort_on_error( "Cannot find file sfx/dig.wav \n Please check your files and try again");

    inventory_hand = new item( 0, 0, hand, hand, -1, "hand");
    inventory_item = inventory_hand;

    // Load fonts
    f1 = load_font("fonts/pixelart2.pcx", NULL, NULL);
    f2 = extract_font_range(f1, ' ', 'A'-1);
    f3 = extract_font_range(f1, 'A', 'Z');
    f4 = extract_font_range(f1, 'Z'+1, 'z');
    pixelart = merge_fonts(f4, f5 = merge_fonts(f2, f3));
    font = merge_fonts(f4, f5 = merge_fonts(f2, f3));

    // Destroy temporary fonts
    destroy_font(f1);
    destroy_font(f2);
    destroy_font(f3);
    destroy_font(f4);
    destroy_font(f5);
}

// Draw character to screen
void character::draw( BITMAP *tempBuffer){
    // Indicator
    if( inventory_item -> id != -1){
        if( inventory_item -> id == 4 || inventory_item -> id == 5){
            if( direction == 1 || direction == 2){
                indicator_x = x;
                indicator_y = y - (16 * ((direction * 2) - 3));
                draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
            }
            else if( direction == 3 || direction == 4){
                indicator_x = x - (16 * ((direction * 2) - 7));
                indicator_y = y;
                draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
            }
        }
        else{
            indicator_x = x;
            indicator_y = y;
            draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
        }
    }

    // Draw frame
    masked_blit( image, tempBuffer, floor(gameTick/4) * 16, (direction - 1) * 20, x - map_pointer -> x, y - map_pointer -> y - 8, 16, 20);
}

// Draw character to screen
void character::drawForeground( BITMAP *tempBuffer){
    // Top of head
    masked_blit( image, tempBuffer, floor(gameTick/4) * 16, (direction - 1) * 20, x - map_pointer -> x, y - map_pointer -> y - 8, 16, 8);

    // Inventory box

    draw_sprite( tempBuffer, inventory_gui, 1, 1);


    if( inventory_item -> image[0] != NULL){
      // Watering can
        if( inventory_item -> id == 3)
          draw_sprite( tempBuffer,watering_can[water],2,2);
        else
          draw_sprite( tempBuffer, inventory_item -> image[0], 2, 2);
    }

    //When gcc don't give no damns
    textprintf_ex( tempBuffer, pixelart, 20, 000000000000000000000000000000000000000000000000000000, makecol(255,255,255), -1, inventory_item -> name.c_str());

    // Money
    draw_sprite( tempBuffer, coin, 190, 10);

    // Allan just died a little inside...
    if(store_open)textprintf_ex( tempBuffer,pixelart,205,5,makecol(0,0,0),-1,"x %i",money);
    else textprintf_ex( tempBuffer,pixelart,205,5,makecol(255,255,255),-1,"x %i",money);


    // Message system
    for( int i = 0; i < MAX_MESSAGES; i++)
        textprintf_ex( tempBuffer,pixelart, 5, i * 10 + 145, makecol(255,255,255),-1,"> %s", (player_messages[i]).c_str());


}

// Push message
void character::push_message( std::string new_message){
    for( int i = 0; i < MAX_MESSAGES - 1; i++)
        player_messages[i] = player_messages[i + 1];

    player_messages[MAX_MESSAGES - 1] = new_message;
    std::cout << player_messages[0];
}

// Update player
void character::update(){
    // Ticker
    tick += 2;

    // Ask joystick for keys
    poll_joystick();

    // Oh
    // Snap
    if( x % 16 == 0 && y % 16 == 0 ){
        //gameTick = 0;
        moving = false;
    }

    // Move item with you
    inventory_item -> x = x;
    inventory_item -> y = y;

    // Close store
    if( store_open == true){
        if( key[KEY_LCONTROL] || joy[0].button[2].b || key[KEY_RCONTROL]){
            store_open = false;
            push_message( "Come again");
        }
    }
    // Move
    else if( !moving){
        if(( key[KEY_UP] || joy[0].stick[0].axis[1].d1)){
            direction = 2;
            if( !map_pointer -> is_solid_at( x, y - 16)){
                moving = true;
                sound_step++;
            }
        }
        else if( (key[KEY_DOWN] || joy[0].stick[0].axis[1].d2)){
            direction = 1;
            if( !map_pointer -> is_solid_at( x, y + 16)){
                moving = true;
                sound_step++;
            }
        }
        else if(( key[KEY_LEFT] || joy[0].stick[0].axis[0].d1)){
            direction = 4;
            if( !map_pointer -> is_solid_at( x - 16, y)){
                moving = true;
                sound_step++;
            }
        }
        else if(( key[KEY_RIGHT] || joy[0].stick[0].axis[0].d2)){
            direction = 3;
            if( !map_pointer -> is_solid_at( x + 16, y)){
                moving = true;
                sound_step++;
            }
        }
        if(sound_step > 1){
          play_sample(step_2,50,125,1300,0);
          sound_step = 0;
        }
        if(sound_step == 1 && moving)
          play_sample(step_1,50,125,1300,0);


        // Pickup
        if(( key[KEY_LCONTROL] || joy[0].button[2].b || key[KEY_RCONTROL]) && tick > 20){
            if(inventory_item -> id == -1){
              tick = 0;
              if( map_pointer -> is_item_at( x, y) == true){

                  play_sample(pickup,255,125,1000,0);

                  inventory_item = map_pointer -> get_item_at( x, y);
                  push_message( "You pick up a " + map_pointer -> get_item_at( x, y) -> name);
                  //map_pointer -> remove_item_at( x, y);
              }
            }
            else{
                play_sample(drop,255,125,1000,0);

                tick = 0;
                inventory_item -> x = x;
                inventory_item -> y = y;
                //map_pointer -> place_item( *inventory_item);
                push_message( "You drop your " + inventory_item -> name);
                inventory_item = inventory_hand;
                std::cout << std::endl;
            }
        }

        // Action button
        if(( key[KEY_SPACE] || joy[0].button[0].b) && tick>10){
            push_message( "");


            // OPEN STORE
            if( map_pointer -> get_tile_at( x, y, BACKGROUND) == 19){
                push_message( "Welcome to Danners Devices");
                store_open = true;
            }
            // Hand
            else if( inventory_item -> id == -1 && map_pointer -> get_tile_at(x,y,BACKGROUND) != 7){
                if( map_pointer -> is_item_at( x, y) == true)
                    push_message( "There is a " + map_pointer -> get_item_at( x, y) -> name + " here");
                else if(tick>20){
                    push_message( "There is nothing of interest here");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Hoe
            else if( inventory_item -> id == 0){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 2){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 18, false);
                    play_sample(hoe,255,125,1000,0);
                }else if(tick>20){
                    push_message( "You can't hoe there");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Scythe
            else if( inventory_item -> id == 1){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 4){
                    map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
                    play_sample(cut_scythe,255,125,1000,0);
                }else if(tick>20){
                    push_message( "You can't cut there");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Berry
            else if( inventory_item -> id == 8){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18)
                    map_pointer -> replace_tile( indicator_x, indicator_y, 30, false);
                else if(tick > 20)
                    push_message( "You must plant in ploughed soil");
                    play_sample(error,255,125,1000,0);
            }
            // Tomato
            else if( inventory_item -> id == 10){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18)
                    map_pointer -> replace_tile( indicator_x, indicator_y, 33, false);
                else if(tick > 20)
                    push_message( "You must plant in ploughed soil");
                    play_sample(error,255,125,1000,0);
            }
            // Carrot
            else if( inventory_item -> id == 12){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18)
                    map_pointer -> replace_tile( indicator_x, indicator_y, 36, false);
                else if(tick > 20)
                    push_message( "You must plant in ploughed soil");
                    play_sample(error,255,125,1000,0);
            }
            // Lavender
            else if( inventory_item -> id == 14){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18)
                    map_pointer -> replace_tile( indicator_x, indicator_y, 39, false);
                else if(tick>20){
                    push_message( "You must plant in ploughed soil");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Watering can
            else if( inventory_item -> id == 3){
                if(map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND) == 7){
                    water = 4;
                    push_message( "Watering can filled");
                    play_sample(water_fill,255,125,1000,0);
                }

                else if(water > 0 && tick > 20){
                    water--;
                    push_message("Watered");
                    play_sample(water_pour,255,125,1000,0);

                    // Berries
                    int wateringID = map_pointer -> get_tile_at( indicator_x, indicator_y, false);
                    if( wateringID == 30 || wateringID == 31 || wateringID == 33 || wateringID == 34 ||
                        wateringID == 36 || wateringID == 37 || wateringID == 39 || wateringID == 40){

                        map_pointer -> replace_tile( indicator_x, indicator_y, wateringID + 1, false);
                    }
                }
                else if(tick>20){
                    push_message("Out of water");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Axe
            else if( inventory_item -> id == 4){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 5){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 11, true);
                    play_sample(cut_axe,255,125,1000,0);
                }else if(tick>20){
                    push_message( "You can't chop that down");
                    play_sample(error,255,125,1000,0);
                }
            }
            // Shovel
            else if( inventory_item -> id == 5){
                //Literally the worst formatted if statement I've seen all week
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 6 ||
                    map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 11){
                    map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
                    play_sample(dig,255,125,1000,0);
                }
                else if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 0
                        && map_pointer -> get_tile_at( indicator_x, indicator_y, true) == -1){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 18, false);
                    play_sample(dig,255,125,1000,0);
                }
                else if(tick > 20){
                    push_message( "You can't dig that up");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            std::cout<<". Tile Back ID: "<<map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND)<<std::endl;
            std::cout<<". Tile Front ID: "<<map_pointer -> get_tile_at( indicator_x, indicator_y, true)<<std::endl;

            tick = 0;
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
        if( gameTick > 15)
            gameTick = 0;
    }
}

// Remove item from hand ( and the world)
void character::remove_item(){
    inventory_item = inventory_hand;
    map_pointer -> remove_item_at( x, y);
}

// Give item
void character::give_item( char newItem){
    map_pointer -> place_new_item_at( x, y, newItem);
}

// World object to point to (needs this!)
void character::setWorld( tile_map *newTileMap){
    map_pointer = newTileMap;
}
