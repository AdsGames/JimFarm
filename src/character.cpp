#include "character.h"

#include <iostream>
#include <math.h>

#include "math.h"
#include "keyListener.h"
#include "mouseListener.h"
#include "tools.h"

character::character(){
  moving = false;
  direction = 1;
  draw_hud = true;

  character_inv = inventory(8);
  selected_item = 0;

  for( int i = 0; i < MAX_MESSAGES; i++)
    player_messages[i] = "";

  money = 2;

  store_open = false;
}

character::~character(){}

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

  // Load fonts
  pixelart = load_font_ex("fonts/pixelart_condensed.pcx");
  font = pixelart;
}

// Draw character to screen
void character::draw( BITMAP *tempBuffer){
  // Indicator
  if( character_inv.getItem(selected_item) != NULL && (character_inv.getItem(selected_item) -> getID() == 4 || character_inv.getItem(selected_item) -> getID() == 5)){
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
    if( character_inv.getItem(selected_item) != NULL)
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
    // Draw items
    for( int i = 0; i < character_inv.getMaxSize(); i++){
      draw_sprite( tempBuffer, inventory_gui, 18 * i + 1, 1);
      if( i == selected_item)
        draw_sprite( tempBuffer, indicator, 18 * i + 2, 2);
      if( character_inv.getItem(i) != NULL){
        character_inv.getItem(i) -> draw( 18 * i + 2, 2, tempBuffer);
        if( i == selected_item)
          textprintf_ex( tempBuffer, pixelart, 2, 22, makecol(255,255,255), -1, character_inv.getItem(i) -> getName().c_str());
      }
    }

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

  // Selected item
  // Draw items
  if( character_inv.getItem(selected_item) != NULL)
    character_inv.getItem(selected_item) -> draw( x - map_pointer -> x, y - map_pointer -> y, tempBuffer);
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

  // Hide Hud
  if(keyListener::keyPressed[KEY_F1])
    draw_hud = !draw_hud;

  // Close store
  if( store_open){
    if( keyListener::keyPressed[KEY_SPACE] || joy[0].button[0].b ){
      // Make sure it is not opened by other code
      keyListener::keyPressed[KEY_SPACE] = false;

      store_open = false;
      push_message( "Come again");
    }
  }
  // Move
  else{
    if( !moving){
      // Selector
      if( keyListener::keyPressed[KEY_O] || mouseListener::mouse_z_change > 0)
        selected_item = (selected_item + (character_inv.getMaxSize() - 1)) % character_inv.getMaxSize();
      if( keyListener::keyPressed[KEY_P] || mouseListener::mouse_z_change < 0)
        selected_item = (selected_item + 1) % character_inv.getMaxSize();

      // Up
      if( key[KEY_UP] || key[KEY_W] || joy[0].stick[0].axis[1].d1){
        direction = 2;
        if( !map_pointer -> solid_at( x, y - 16)){
          moving = true;
          sound_step++;
        }
      }
      // Down
      else if( key[KEY_DOWN] || key[KEY_S] || joy[0].stick[0].axis[1].d2){
        direction = 1;
        if( !map_pointer -> solid_at( x, y + 16)){
          moving = true;
          sound_step++;
        }
      }
      // Left
      else if( key[KEY_LEFT] || key[KEY_A] || joy[0].stick[0].axis[0].d1){
        direction = 4;
        if( !map_pointer -> solid_at( x - 16, y)){
          moving = true;
          sound_step++;
        }
      }
      // Right
      else if( key[KEY_RIGHT] || key[KEY_D] || joy[0].stick[0].axis[0].d2){
        direction = 3;
        if( !map_pointer -> solid_at( x + 16, y)){
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
      if( keyListener::keyPressed[KEY_LCONTROL] || mouseListener::mouse_pressed & 1 || joy[0].button[2].b ){
        item *itemAtPos  = map_pointer -> item_at( indicator_x, indicator_y);
        item *itemInHand = character_inv.getItem(selected_item);

        if( itemInHand == NULL && itemAtPos != NULL){
          play_sample( pickup, 255, 125, 1000, 0);
          if( character_inv.addItem( itemAtPos, selected_item)){
            map_pointer -> remove_item( itemAtPos);
            push_message( "You pick up a " + character_inv.getItem(selected_item) -> getName());
          }
        }
        else if( itemInHand != NULL && itemAtPos == NULL){
          play_sample( drop, 255, 125, 1000, 0);
          map_pointer -> place_item( character_inv.getItem(selected_item), x, y);

          if( character_inv.removeItem( selected_item))
            push_message( "You drop your " + itemInHand -> getName());
        }
      }

      // Action button
      if( keyListener::keyPressed[KEY_SPACE] || joy[0].button[0].b){
        push_message( "");

        tile *foregroundTile = map_pointer -> tile_at( indicator_x, indicator_y, FOREGROUND);
        tile *backgroundTile = map_pointer -> tile_at( indicator_x, indicator_y, BACKGROUND);

        if( foregroundTile != NULL)
          std::cout << ". Tile Front ID: "<< foregroundTile -> getID() << std::endl;
        if( backgroundTile != NULL)
          std::cout << ". Tile Back ID: " <<  backgroundTile -> getID() << std::endl;

        // OPEN STORE
        if( backgroundTile != NULL && backgroundTile -> getID() == 19){
          push_message( "Welcome to Danners Devices");
          store_open = true;
        }
        // Hand
        else if( character_inv.getItem(selected_item) == NULL && backgroundTile != NULL && backgroundTile -> getID() != 7){
          if( map_pointer -> item_at( indicator_x, indicator_y))
            push_message( "There is a " + map_pointer -> item_at( indicator_x, indicator_y) -> getName() + " here");
          else{
            push_message( "There is nothing of interest here");
            play_sample( error, 255, 125, 1000, 0);
          }
        }
        else if( character_inv.getItem( selected_item) != NULL){
          // Hoe
          if( character_inv.getItem(selected_item) -> getID() == 1){
            if( backgroundTile != NULL && backgroundTile -> getID() == 2){
              map_pointer -> replace_tile( indicator_x, indicator_y, 18, false);
              play_sample(hoe,255,125,1000,0);
            }
            else {
              push_message( "You can't hoe there");
              play_sample( error, 255, 125, 1000, 0);
            }
          }
          // Scythe
          else if( character_inv.getItem(selected_item) -> getID() == 2){
            if( foregroundTile != NULL && foregroundTile -> getID() == 4){
              map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
              play_sample(cut_scythe,255,125,1000,0);
            }
            else {
              push_message( "You can't cut there");
              play_sample( error, 255, 125, 1000, 0);
            }
          }
          // Berry
          else if( character_inv.getItem(selected_item) -> getID() == 8){
            if( backgroundTile != NULL && backgroundTile -> getID() == 18){
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
          else if( character_inv.getItem(selected_item) -> getID() == 10){
            if( backgroundTile != NULL && backgroundTile -> getID() == 18){
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
          else if( character_inv.getItem(selected_item) -> getID() == 12){
            if( backgroundTile != NULL && backgroundTile -> getID() == 18){
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
          else if( character_inv.getItem(selected_item) -> getID() == 14){
            if( backgroundTile != NULL && backgroundTile -> getID() == 18){
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
          else if( character_inv.getItem(selected_item) -> getID() == 3){
            if( backgroundTile != NULL && backgroundTile -> getID() == 7){
              water = 4;
              push_message( "Watering can filled");
              play_sample( water_fill, 255, 125, 1000, 0);
            }

            else if(water > 0){
              water--;
              push_message("Watered");
              play_sample( water_pour, 255, 125, 1000, 0);

              // Berries
              if( backgroundTile != NULL){
                int wateringID = backgroundTile -> getID();
                if( wateringID == 30 || wateringID == 31 || wateringID == 33 || wateringID == 34 ||
                  wateringID == 36 || wateringID == 37 || wateringID == 39 || wateringID == 40){

                  map_pointer -> replace_tile( indicator_x, indicator_y, wateringID + 1, false);
                }
              }
            }
            else{
              push_message("Out of water");
              play_sample( error, 255, 125, 1000, 0);
            }
          }
          // Axe
          else if( character_inv.getItem(selected_item) -> getID() == 4){
            if( foregroundTile != NULL && foregroundTile  -> getID() == 5){
              map_pointer -> replace_tile( indicator_x, indicator_y, 11, true);
              play_sample( cut_axe, 255, 125, 1000, 0);
            }
            else{
              push_message( "You can't chop that down");
              play_sample( error, 255, 125, 1000, 0);
            }
          }
          // Shovel
          else if( character_inv.getItem(selected_item) -> getID() == 5){
            //Literally the worst formatted if statement I've seen all week
            if( foregroundTile != NULL && (foregroundTile  -> getID() == 6 || foregroundTile  -> getID() == 11)){
              map_pointer -> replace_tile( indicator_x, indicator_y, -1, true);
              play_sample(dig,255,125,1000,0);
            }
            else if(  backgroundTile != NULL && backgroundTile -> getID() == 0 && foregroundTile == NULL){
              map_pointer -> replace_tile( indicator_x, indicator_y, 2,false);
              play_sample(dig,255,125,1000,0);
            }
            else{
              push_message( "You can't dig that up");
              play_sample( error, 255, 125, 1000, 0);
            }
          }
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
      if( gameTick > 15)
        gameTick = 0;
    }
}

// Remove item from hand ( and the world)
void character::remove_item(){
  //inventory_item = inventory_hand;
  map_pointer -> remove_item_at( x, y);
}

// Give item
void character::give_item( char newItem){
  map_pointer -> place_item( new item( newItem), x, y);
  character_inv.addItem( map_pointer -> item_at( x, y), selected_item);
}

// World object to point to (needs this!)
void character::setWorld( tile_map *newTileMap){
  map_pointer = newTileMap;
}
