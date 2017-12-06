#include "tile_map.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>

#include "tools.h"
#include "tile_type_manager.h"
#include "tile_defs.h"
#include "item_defs.h"

#include "sound_manager.h"
#include "sound_defs.h"

bool comparePtrToNode(tile *a, tile *b){
  return (*a < *b);
}

/*************
 * MAP ITEMS *
 *************/
map_item::map_item(){
  this -> x = 0;
  this -> y = 0;
  this -> itemPtr = NULL;
}

map_item::map_item( int x, int y, item *itemPtr){
  this -> x = x;
  this -> y = y;
  this -> itemPtr = itemPtr;
}

void map_item::draw( BITMAP *tempBuffer){
  itemPtr -> draw( x, y, tempBuffer);
}



/************
 * TILE MAP *
 ************/
tile_map::tile_map(){
  x = 128;
  y = 0;

  ticks = 0;

  map_messages = new messenger( 1, false, -4);
}


// Ticker
tile_map::~tile_map(){}
volatile int tile_map::ticks = 0;
void tile_map::tick_counter(){
  ticks++;
}
END_OF_FUNCTION(tick_counter)

// Init ticker to 20 ticks per second
void tile_map::init_ticker(){
  LOCK_VARIABLE( ticks);
  LOCK_FUNCTION( tick_counter);

  install_int_ex( tick_counter, BPS_TO_TIMER(20));
}

/*
 * IMAGES
 */
// Draw tiles
void tile_map::draw( BITMAP *tempBuffer){
  // Draw tiles
  for( unsigned int i = 0; i < map_tiles.size(); i++)
    map_tiles.at(i) -> draw( map_buffer);

  blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

// Draw tiles
void tile_map::drawForeground( BITMAP *tempBuffer){
  rectfill( map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol( 255, 0, 255));

  // Draw items
  for( unsigned int i = 0; i < map_items.size(); i++)
    map_items.at(i) -> draw( map_buffer);

  // Draw foreground
  for( unsigned int i = 0; i < map_tiles_foreground.size(); i++)
    map_tiles_foreground.at(i) -> draw( map_buffer);

  masked_blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  // Message system
  map_messages -> draw( tempBuffer, 5, 145);
}

// Load images
void tile_map::load_images(){
  tile_type_manager::sprite_sheet_tiles = load_bitmap_ex( "images/tiles.png");
  tile_type_manager::sprite_sheet_items = load_bitmap_ex( "images/items.png");

  tile_type_manager::load("data/tiles.xml", false);
  tile_type_manager::load("data/items.xml", true);
  sound_manager::load("data/sounds.xml");
}


/*
 * TILES
 */
// Get tile at position
tile *tile_map::tile_at( int positionX, int positionY, bool foreground){
  std::vector<tile*> *layer = &map_tiles;
  if( foreground)
    layer = &map_tiles_foreground;

  for( unsigned int i = 0; i < layer -> size(); i++)
    if( (layer -> at(i) -> getX() <= positionX) && (layer -> at(i) -> getX() + layer -> at(i) -> getWidth()  > positionX) &&
        (layer -> at(i) -> getY() - layer -> at(i) -> getHeight() < positionY) && (layer -> at(i) -> getY() >= positionY))
      return layer -> at(i);

  return NULL;
}

// Place tile on map
void tile_map::place_tile( tile* newTile){
  if( newTile != NULL)
    map_tiles.push_back( newTile);
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID, bool foreground){
  std::vector<tile*> *layer = &map_tiles;
  if( foreground)
    layer = &map_tiles_foreground;

  for( unsigned int i = 0; i < layer -> size(); i++){
    if( layer -> at(i) -> getX() == tileX && layer -> at(i) -> getY() == tileY){
      if( newID == -1)
        layer -> erase( layer -> begin() + i);
      else{
        layer -> at(i) -> setID( newID);
      }
    }
  }
}

// Check for solid tile
bool tile_map::solid_at( int positionX, int positionY){
  if( tile_at( positionX, positionY, true))
    return tile_at( positionX, positionY, true) -> isSolid();
  return false;
}


/*
 * ITEMS
 */
// Get item at position
item *tile_map::item_at( int positionX, int positionY){
  for( unsigned int i = 0; i < map_items.size(); i++)
    if( map_items.at(i) -> x == positionX && map_items.at(i) -> y == positionY)
      return map_items.at(i) -> itemPtr;
  return NULL;
}

// Place item on map
void tile_map::place_item( item* newItem, int x, int y){
  if( newItem != NULL){
    map_item *newMapItem = new map_item( x, y, newItem);
    map_items.push_back( newMapItem);
  }
}

// Remove item by value
void tile_map::remove_item( item *newItem){
  for( unsigned int i = 0; i < map_items.size(); i++){
    if( map_items.at(i) -> itemPtr == newItem){
      map_items.erase( map_items.begin() + i);
      break;
    }
  }
}

// Pick up item at position
void tile_map::remove_item_at( int positionX, int positionY){
  item *tempItem = item_at( positionX, positionY);
  if( tempItem != NULL)
    remove_item( tempItem);
}


/*
 * MAP
 */
// Interact with
void tile_map::interact( int inter_x, int inter_y, item *inHand){
  tile *foregroundTile = tile_at( inter_x, inter_y, FOREGROUND);
  tile *backgroundTile = tile_at( inter_x, inter_y, BACKGROUND);

  // Hoe
  if( inHand -> getID() == ITEM_HOE){
    if( backgroundTile && !foregroundTile){
      if( backgroundTile -> getID() == TILE_GRASS){
        replace_tile( inter_x, inter_y, TILE_SOIL, false);
        sound_manager::play( SOUND_HOE);
      }
      else if( backgroundTile -> getID() == TILE_SOIL){
        replace_tile( inter_x, inter_y, TILE_PLOWED_SOIL, false);
        sound_manager::play( SOUND_HOE);
      }
      else {
        map_messages -> push_message( "You can't hoe that");
      }
    }
    else {
      map_messages -> push_message( "You can't hoe there");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Scythe
  else if( inHand -> getID() == ITEM_SCYTHE){
    if( foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS){
      replace_tile( inter_x, inter_y, -1, true);
      sound_manager::play( SOUND_SCYTHE);
    }
    else {
      map_messages -> push_message( "You can't cut there");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Berry
  else if( inHand -> getID() == ITEM_BERRY_SEED){
    if( backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL){
      replace_tile( inter_x, inter_y, TILE_BERRY, false);
    }
    else {
      map_messages -> push_message( "You must plant in ploughed soil");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Tomato
  else if( inHand -> getID() == ITEM_TOMATO_SEED){
    if( backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL){
      replace_tile( inter_x, inter_y, TILE_TOMATO, false);
    }
    else{
      map_messages -> push_message( "You must plant in ploughed soil");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Carrot
  else if( inHand -> getID() == ITEM_CARROT_SEED){
    if( backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL){
      replace_tile( inter_x, inter_y, TILE_CARROT, false);
    }
    else{
      map_messages -> push_message( "You must plant in ploughed soil");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Lavender
  else if( inHand -> getID() == ITEM_LAVENDER_SEED){
    if( backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL){
      replace_tile( inter_x, inter_y, TILE_LAVENDER, false);
    }
    else{
      map_messages -> push_message( "You must plant in ploughed soil");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Watering can
  /*else if( character_inv.getItem(selected_item) -> getID() == ITEM_WATERING_CAN){
    if( backgroundTile && backgroundTile -> getID() == TILE_WELL_PATH){
      water = 4;
      push_message( "Watering can filled");
      sound_manager::play( SOUND_WATER_FILL);
    }

    else if(water > 0){
      water--;
      push_message("Watered");
      sound_manager::play( SOUND_WATER_POUR);

      // Berries
      if( backgroundTile){
        int wateringID = backgroundTile -> getID();
        if( wateringID == TILE_BERRY_1 || wateringID == TILE_BERRY_2 || wateringID == TILE_BERRY_1 || wateringID == TILE_BERRY_2 ||
          wateringID == TILE_CARROT_1 || wateringID == TILE_CARROT_2 || wateringID == TILE_LAVENDER_1 || wateringID == TILE_LAVENDER_2){

          map_pointer -> replace_tile( indicator_x, indicator_y, wateringID + 1, false);
        }
      }
    }
    else{
      push_message("Out of water");
      sound_manager::play( SOUND_ERROR);
    }
  }*/
  // Axe
  else if( inHand -> getID() == ITEM_AXE){
    if( foregroundTile && foregroundTile  -> getID() == TILE_TREE){
      replace_tile( inter_x, inter_y, TILE_STUMP, true);
      sound_manager::play( SOUND_AXE);
    }
    else{
      map_messages -> push_message( "You can't chop that down");
      sound_manager::play( SOUND_ERROR);
    }
  }
  // Shovel
  else if( inHand -> getID() == ITEM_SHOVEL){
    //Literally the worst formatted if statement I've seen all week
    if( foregroundTile && (foregroundTile  -> getID() == TILE_BUSH || foregroundTile  -> getID() == TILE_STUMP)){
      replace_tile( inter_x, inter_y, TILE_NULL, true);
      sound_manager::play( SOUND_SHOVEL);
    }
    else if(  backgroundTile && backgroundTile -> getID() == TILE_GRASS && foregroundTile){
      replace_tile( inter_x, inter_y, 2,false);
      sound_manager::play( SOUND_SHOVEL);
    }
    else{
      map_messages -> push_message( "You can't dig that up");
      sound_manager::play( SOUND_ERROR);
    }
  }
}

// Update tile map
void tile_map::update(){
  if( ticks >= 1){
    ticks = 0;

    // Run tickers for itens
    for( unsigned int i = 0; i < map_items.size(); i++){
      // Current item
      item *current = map_items.at(i) -> itemPtr;

      // Tile near item
      tile *foregroundTile = tile_at( map_items.at(i) -> x, map_items.at(i) -> y, FOREGROUND);
      tile *backgroundTile = tile_at( map_items.at(i) -> x, map_items.at(i) -> y, BACKGROUND);

      // Chicken eggs
      if( current -> getID() == ITEM_CHICKEN){
        if( backgroundTile && backgroundTile -> getID() == TILE_PATCHY_GRASS){
          if( !random(0,16)){
            int rand_1 = 16 * random( -1, 1);
            int rand_2 = 16 * random( -1, 1);
            if( !item_at( map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2) &&
                !solid_at( map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2)){
              place_item( new item( ITEM_EGG), map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2);
              sound_manager::play( SOUND_EGG);
            }
          }
        }
      }
    }

    // Run tickers for tiles
    for( unsigned int i = 0; i < map_tiles.size(); i++){
      // Current tile
      tile *current = map_tiles.at(i);

      // Animations
      if( current -> getID() == TILE_WATER){
        current -> addMeta(8);
      }

      // Berries
      if( current -> getID() == TILE_BERRY){
        // Grow a bit
        if( true)
          current -> addMeta(1);
        // Done Growing
        if( current -> getMeta() >= MAX_TILE_META){
          place_item( new item(ITEM_BERRY), current -> getX(), current -> getY());
          replace_tile( current -> getX(), current -> getY(), TILE_SOIL, false);
        }
      }
      // Tomatos
      if( current -> getID() == TILE_TOMATO){
        // Grow a bit
        if( !random( 0, 2))
          current -> addMeta(1);
        // Done Growing
        if( current -> getMeta() >= MAX_TILE_META){
          place_item( new item(ITEM_TOMATO), current -> getX(), current -> getY());
          replace_tile( current -> getX(), current -> getY(), TILE_SOIL, false);
        }
      }
      // Carrots
      if( current -> getID() == TILE_CARROT){
        // Grow a bit
        if( !random( 0, 5))
          current -> addMeta(1);
        // Done Growing
        if( current -> getMeta() >= MAX_TILE_META){
          place_item( new item(ITEM_CARROT), current -> getX(), current -> getY());
          replace_tile( current -> getX(), current -> getY(), TILE_SOIL, false);
        }
      }
      // Lavender
      if( current -> getID() == TILE_LAVENDER){
        // Grow a bit
        if( !random( 0, 10))
          current -> addMeta(1);
        // Done Growing
        if( current -> getMeta() >= MAX_TILE_META){
          place_item( new item(ITEM_LAVENDER), current -> getX(), current -> getY());
          replace_tile( current -> getX(), current -> getY(), TILE_SOIL, false);
        }
      }
    }
  }
}

// Scroll
void tile_map::scroll( int player_x, int player_y){
  // Scroll x
  if( (player_x + 64) - x > VIEWPORT_WIDTH && x + VIEWPORT_WIDTH < MAP_WIDTH * 16)
    x += 2;
  else if( (player_x - 64) - x < 0 && x > 0)
    x -= 2;

  // Scroll y
  if( (player_y + 48) - y > VIEWPORT_HEIGHT && y + VIEWPORT_HEIGHT < MAP_HEIGHT * 16)
    y += 2;
  else if( (player_y - 48) - y < 0 && y > 0)
    y -= 2;
}

// Generate map
void tile_map::generate_map(){
  load_map( "data/map");

  int tempMapForeground[MAP_WIDTH][MAP_HEIGHT];

  // Generate map
  for( int i = 0; i < MAP_WIDTH; i++){
    for( int t = 0; t < MAP_HEIGHT; t++){
      tempMapForeground[i][t] = TILE_NULL;

      // Dense grass
      if( random( 0, 10) == 0){
        tempMapForeground[i][t] = TILE_DENSE_GRASS;
      }
      // Trees
      else if( random( 0, 30) == 0){
        tempMapForeground[i][t] = TILE_TREE;
      }
      // Bushes
      else if( random( 0, 20) == 0){
        tempMapForeground[i][t] = TILE_BUSH;
      }
    }
  }

  // Grow Grass
  for( int i = 0; i < MAP_WIDTH; i++){
    // Who the frick uses t as an iterator?
    // Allan, that's who!
    for( int t = 0; t < MAP_HEIGHT; t++){
      if( tempMapForeground[i][t] == TILE_DENSE_GRASS){
        if( i > 0 && random( 0, 2) == 0 && tempMapForeground[i][t] == TILE_NULL){
          tempMapForeground[i - 1][t] = TILE_DENSE_GRASS;
        }
        if( i < MAP_WIDTH - 1 && random( 0, 2) == 0 && tempMapForeground[i][t] == TILE_NULL){
          tempMapForeground[i + 1][t] = TILE_DENSE_GRASS;
        }
        if( t > 0 && random( 0, 2) == 0 && tempMapForeground[i][t] == TILE_NULL){
          tempMapForeground[i][t - 1] = TILE_DENSE_GRASS;
        }
        if( t < MAP_HEIGHT - 1 && random( 0, 2) == 0 && tempMapForeground[i][t] == TILE_NULL){
          tempMapForeground[i][t + 1] = TILE_DENSE_GRASS;
        }
      }
    }
  }

  // Spread Trees
  for( int j = 0; j < 5; j++){
    for( int i = 0; i < MAP_WIDTH; i++){
      for( int t = 0; t < MAP_HEIGHT; t++){
        if( tempMapForeground[i][t] == TILE_TREE){
          if( i > 0 && random( 0, 5) == 0  && tempMapForeground[i - 1][t] == TILE_NULL){
            tempMapForeground[i - 1][t] = TILE_TREE;
          }
          if( i < MAP_WIDTH - 1 && random( 0, 5) == 0 && tempMapForeground[i + 1][t] == TILE_NULL){
            tempMapForeground[i + 1][t] = TILE_TREE;
          }
          if( t > 0 && random( 0, 5) == 0 && tempMapForeground[i][t - 1] == TILE_NULL){
            tempMapForeground[i][t - 1] = TILE_TREE;
          }
          if( t < MAP_HEIGHT - 1 && random( 0, 5) == 0 && tempMapForeground[i][t + 1] == TILE_NULL){
            tempMapForeground[i][t + 1] = TILE_TREE;
          }
        }
      }
    }
  }

  // Turn numbers into objects
  for( int i = 0; i < MAP_WIDTH; i++){
    for( int t = 0; t < MAP_HEIGHT; t++){
      tile *backgroundTile = tile_at( i * 16, t * 16, BACKGROUND);
      tile *foregroundTile = tile_at( i * 16, t * 16, FOREGROUND);
      if( backgroundTile && backgroundTile -> getID() == TILE_GRASS && !foregroundTile){
        if( tempMapForeground[i][t] != TILE_NULL){
          tile* newTile2 = new tile( i * 16, t * 16, tempMapForeground[i][t]);
          map_tiles_foreground.push_back( newTile2);
        }
      }
    }
  }

  // Place hoe ( 1)
  place_item( new item(ITEM_HOE), 17 * 16, 5 * 16);

  place_item( new item(ITEM_CARROT_SEED), 18 * 16, 5 * 16);
  place_item( new item(ITEM_LAVENDER_SEED), 19 * 16, 5 * 16);
  place_item( new item(ITEM_BERRY_SEED), 20 * 16, 5 * 16);
  place_item( new item(ITEM_TOMATO_SEED), 21 * 16, 5 * 16);

  // Place chickens (4)
  int placed = 100;
  while( placed < 3){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    tile *foregroundTile = tile_at( random_x, random_y, FOREGROUND);
    if( foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS){
      place_item( new item( ITEM_CHICKEN), random_x, random_y);
      placed += 1;
    }
  }

  placed = 110;
  while( placed < 35){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    if(!solid_at( random_x, random_y)){
      place_item( new item( ITEM_STICK), random_x, random_y );
      placed += 1;
    }
  }


  // SORT IT OUT!
  std::sort( map_tiles_foreground.begin(), map_tiles_foreground.end(), comparePtrToNode);

  // Create map buffer
  map_buffer = create_bitmap( MAP_WIDTH * 16, MAP_HEIGHT * 16);
}

// Manually load new file
void tile_map::load_map( std::string fileName){
  // Loading
  std::cout << "   MAPS\n-------------\n";

  //Change size
  std::string fileLoad = fileName + ".txt";
  std::ifstream findSize(fileLoad.c_str());
  MAP_WIDTH = 0;
  MAP_HEIGHT = 0;
  int data;
  while (findSize >> data) {
    if(MAP_HEIGHT == 0){
        MAP_WIDTH++;
    }
    if(findSize.peek() == '\n'){
        MAP_HEIGHT++;
    }
  }

  //Setup Map
  if(fileName != "blank"){
    map_tiles.clear();
    map_tiles_foreground.clear();

    fileLoad = fileName + ".txt";
    std::ifstream read(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for( int t = 0; t < MAP_HEIGHT; t++){
      for (int i = 0; i < MAP_WIDTH; i++){
        int newTileType;
        read >> newTileType;
        //std::cout << newTileType;
        // Set tile type
        tile* newTile = new tile( i * 16, t * 16, newTileType);
        map_tiles.push_back( newTile);
      }
    }
    read.close();

    fileLoad = fileName + "_back.txt";
    std::ifstream read2(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for( int t = 0; t < MAP_HEIGHT; t++){
      for (int i = 0; i < MAP_WIDTH; i++){
        int newTileType;
        read2 >> newTileType;
        //std::cout << newTileType;
        // Set tile type
        if( newTileType != TILE_NULL){
          tile* newTile = new tile( i * 16, t * 16, newTileType);
          map_tiles_foreground.push_back( newTile);
        }
      }
    }
    read2.close();
  }
  std::cout << "\n\n";
}
