#include "character.h"

character::character(){
    moving = false;
    direction = 1;
    draw_hud = true;

    for( int i = 0; i < MAX_MESSAGES; i++)
        player_messages[i] = "";

    money = 2;

    store_open = false;
}

character::~character(){
    //dtor
}

// Set image
void character::load_data(){
    // Images
    image = load_bitmap_ex("images/character_1.png");
    watering_can[0] = load_bitmap_ex("images/watering_can_0.png");
    watering_can[1] = load_bitmap_ex("images/watering_can_1.png");
    watering_can[2] = load_bitmap_ex("images/watering_can_2.png");
    watering_can[3] = load_bitmap_ex("images/watering_can_3.png");
    watering_can[4] = load_bitmap_ex("images/watering_can_4.png");
    inventory_gui = load_bitmap_ex("images/GUI_INVENTORY.png");
    hand = load_bitmap_ex("images/hand.png");
    indicator = load_bitmap_ex("images/indicator.png");
    coin = load_bitmap_ex("images/coin.png");

    // Sounds
     pickup = load_sample_ex("sfx/pickup.wav");
    drop = load_sample_ex("sfx/drop.wav");
    step_1 = load_sample_ex("sfx/step_1.wav");
    step_2 = load_sample_ex("sfx/step_2.wav");
    cut_axe = load_sample_ex("sfx/cut_axe.wav");
    cut_scythe = load_sample_ex("sfx/cut_scythe.wav");
    hoe = load_sample_ex("sfx/hoe.wav");
    error = load_sample_ex("sfx/error.wav");
    water_fill = load_sample_ex("sfx/water_fill.wav");
    water_pour = load_sample_ex("sfx/water_pour.wav");
    dig = load_sample_ex("sfx/dig.wav");



    inventory_hand = new item( 0, 0, 0);
    inventory_item = inventory_hand;

    // Load fonts
    pixelart = load_font_ex("fonts/pixelart_condensed.pcx");
    font = pixelart;
}

// Draw character to screen
void character::draw( BITMAP *tempBuffer){
    // Indicator
    if( inventory_item -> getID() == 4 || inventory_item -> getID() == 5){
        if( direction == 1 || direction == 2){
            indicator_x = (x/16)*16;
            indicator_y = (y/16)*16 - (16 * ((direction * 2) - 3));
            draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
        }
        else if( direction == 3 || direction == 4){
            indicator_x = (x/16)*16 - (16 * ((direction * 2) - 7));
            indicator_y = (y/16)*16;
            draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
        }
    }
    else{
        indicator_x = (x/16)*16;
        indicator_y = (y/16)*16;

        // Only show if hands arent empty
        if( inventory_item -> getID() != 0)
          draw_sprite( tempBuffer, indicator, indicator_x - map_pointer -> x, indicator_y - map_pointer -> y);
    }

    // Draw frame
    masked_blit( image, tempBuffer, floor(gameTick/4) * 16, (direction - 1) * 20, x - map_pointer -> x, y - map_pointer -> y - 8, 16, 20);
}

// Draw character to screen
void character::drawForeground( BITMAP *tempBuffer){
    // Top of head
    masked_blit( image, tempBuffer, floor(gameTick/4) * 16, (direction - 1) * 20, x - map_pointer -> x, y - map_pointer -> y - 8, 16, 8);

    // Inventory box
    if(draw_hud){
      draw_sprite( tempBuffer, inventory_gui, 1, 1);


      // Watering can
      if( inventory_item -> getID() == 3)
        draw_sprite( tempBuffer, watering_can[water], 2, 2);
      else
        inventory_item -> draw_at( 2, 2, tempBuffer);

      //When gcc don't give no damns
      textprintf_ex( tempBuffer, pixelart, 20, 000000000000000000000000000000000000000000000000000000, makecol(255,255,255), -1, inventory_item -> getName().c_str());

      // Money
      draw_sprite( tempBuffer, coin, 190, 10);

      // Allan just died a little inside...
      if(store_open)
        textprintf_ex( tempBuffer,pixelart,205,5,makecol(0,0,0),-1,"x %i",money);
      else
        textprintf_ex( tempBuffer,pixelart,205,5,makecol(255,255,255),-1,"x %i",money);


      // Message system
      for( int i = 0; i < MAX_MESSAGES; i++)
          textprintf_ex( tempBuffer,pixelart, 5, i * 10 + 145, makecol(255,255,255),-1,"> %s", (player_messages[i]).c_str());
    }
}

// Push message
void character::push_message( std::string new_message){
    for( int i = 0; i < MAX_MESSAGES - 1; i++)
        player_messages[i] = player_messages[i + 1];

    player_messages[MAX_MESSAGES - 1] = new_message;
    std::cout << player_messages[0] << "\n";
}

// Update player
void character::update(){
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

    if(keyListener::keyPressed[KEY_F1]){
      draw_hud =! draw_hud;
    }
    // Close store
    if( store_open == true){
        if( keyListener::keyPressed[KEY_SPACE] || joy[0].button[0].b ){
            // Make sure it is not opened by other code
            keyListener::keyPressed[KEY_SPACE] = false;

            store_open = false;
            push_message( "Come again");
        }
    }

    // Move
    if( !store_open){
        if( !moving){
          // Up
          if(( key[KEY_UP] || joy[0].stick[0].axis[1].d1)){
              direction = 2;
              if( !map_pointer -> is_solid_at( x, y - 16)){
                  moving = true;
                  sound_step++;
              }
          }
          // Down
          else if( (key[KEY_DOWN] || joy[0].stick[0].axis[1].d2)){
              direction = 1;
              if( !map_pointer -> is_solid_at( x, y + 16)){
                  moving = true;
                  sound_step++;
              }
          }
          // Left
          else if(( key[KEY_LEFT] || joy[0].stick[0].axis[0].d1)){
              direction = 4;
              if( !map_pointer -> is_solid_at( x - 16, y)){
                  moving = true;
                  sound_step++;
              }
          }
          // Right
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
        }

        // Pickup
        if( keyListener::keyPressed[KEY_LCONTROL] || keyListener::keyPressed[KEY_RCONTROL] || mouse_b & 1 || joy[0].button[2].b ){
          if( inventory_item -> getID() == 0){
            if( map_pointer -> is_item_at( indicator_x, indicator_y) == true){
              play_sample( pickup, 255, 125, 1000, 0);

              inventory_item = map_pointer -> get_item_at( indicator_x, indicator_y);
              push_message( "You pick up a " + inventory_item -> getName());
            }
          }
          else{
            play_sample( drop, 255, 125, 1000, 0);
            inventory_item -> x = indicator_x;
            inventory_item -> y = indicator_y;
            push_message( "You drop your " + inventory_item -> getName());

            inventory_item = inventory_hand;
            std::cout << std::endl;
          }
        }

        // Action button
        if( keyListener::keyPressed[KEY_SPACE] || joy[0].button[0].b){
            push_message( "");

            // OPEN STORE
            if( map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND) == 19){
                push_message( "Welcome to Danners Devices");
                store_open = true;
            }
            // Hand
            else if( inventory_item -> getID() == 0 && map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND) != 7){
                if( map_pointer -> is_item_at( indicator_x, indicator_y) == true)
                    push_message( "There is a " + map_pointer -> get_item_at( indicator_x, indicator_y) -> getName() + " here");
                else{
                    push_message( "There is nothing of interest here");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Hoe
            else if( inventory_item -> getID() == 1){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 2){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 18, false);
                    play_sample(hoe,255,125,1000,0);
                }
                else {
                    push_message( "You can't hoe there");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Scythe
            else if( inventory_item -> getID() == 2){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 4){
                    map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
                    play_sample(cut_scythe,255,125,1000,0);
                }
                else {
                    push_message( "You can't cut there");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Berry
            else if( inventory_item -> getID() == 8){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 30, false);
                    if( random( 0, 2) == 0){
                        remove_item();
                    }
                }
                else {
                    push_message( "You must plant in ploughed soil");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Tomato
            else if( inventory_item -> getID() == 10){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 33, false);
                    if( random( 0, 2) == 0){
                        remove_item();
                    }
                }
                else{
                    push_message( "You must plant in ploughed soil");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Carrot
            else if( inventory_item -> getID() == 12){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 36, false);
                    if( random( 0, 2) == 0){
                        remove_item();
                    }
                }
                else{
                    push_message( "You must plant in ploughed soil");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Lavender
            else if( inventory_item -> getID() == 14){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 18){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 39, false);
                    if( random( 0, 2) == 0){
                        remove_item();
                    }
                }
                else{
                    push_message( "You must plant in ploughed soil");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Watering can
            else if( inventory_item -> getID() == 3){
                if(map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND) == 7){
                    water = 4;
                    push_message( "Watering can filled");
                    play_sample( water_fill, 255, 125, 1000, 0);
                }

                else if(water > 0){
                    water--;
                    push_message("Watered");
                    play_sample( water_pour, 255, 125, 1000, 0);

                    // Berries
                    int wateringID = map_pointer -> get_tile_at( indicator_x, indicator_y, false);
                    if( wateringID == 30 || wateringID == 31 || wateringID == 33 || wateringID == 34 ||
                        wateringID == 36 || wateringID == 37 || wateringID == 39 || wateringID == 40){

                        map_pointer -> replace_tile( indicator_x, indicator_y, wateringID + 1, false);
                    }
                }
                else{
                    push_message("Out of water");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Axe
            else if( inventory_item -> getID() == 4){
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 5){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 11, true);
                    play_sample( cut_axe, 255, 125, 1000, 0);
                }
                else{
                    push_message( "You can't chop that down");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            // Shovel
            else if( inventory_item -> getID() == 5){
                //Literally the worst formatted if statement I've seen all week
                if( map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 6
                      || map_pointer -> get_tile_at( indicator_x, indicator_y, true) == 11){
                    map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
                    play_sample(dig,255,125,1000,0);
                }
                else if( map_pointer -> get_tile_at( indicator_x, indicator_y, false) == 0
                      && map_pointer -> get_tile_at( indicator_x, indicator_y, true) == -1){
                    map_pointer -> replace_tile( indicator_x, indicator_y, 2,false);
                    play_sample(dig,255,125,1000,0);
                }
                else{
                    push_message( "You can't dig that up");
                    play_sample( error, 255, 125, 1000, 0);
                }
            }
            std::cout << ". Tile Back ID: " << map_pointer -> get_tile_at( indicator_x, indicator_y, BACKGROUND) << std::endl;
            std::cout << ". Tile Front ID: "<< map_pointer -> get_tile_at( indicator_x, indicator_y, true) << std::endl;
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
    inventory_item = map_pointer -> get_item_at( x, y);

}

// World object to point to (needs this!)
void character::setWorld( tile_map *newTileMap){
    map_pointer = newTileMap;
}
