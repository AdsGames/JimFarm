#include "tile_map.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>

#include "tools.h"
#include "tile_type_manager.h"

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
}

tile_map::~tile_map(){}


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
}

// Load images
void tile_map::load_images(){
  tile_type_manager::sprite_sheet_tiles = load_bitmap_ex( "images/tiles.png");
  tile_type_manager::sprite_sheet_items = load_bitmap_ex( "images/items.png");

  tile_type_manager::load("data/tiles.xml", false);
  tile_type_manager::load("data/items.xml", true);

  egg = load_sample_ex("sfx/egg.wav");
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
    if( layer -> at(i) -> x == positionX && layer -> at(i) -> y == positionY)
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
    if( layer -> at(i) -> x == tileX && layer -> at(i) -> y == tileY){
      if( newID == -1)
        layer -> erase( layer -> begin() + i);
      else{
        layer -> at(i) -> setID( newID);
        layer -> at(i) -> requirements_met = false;
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


/*
 * MAP
 */
// Pick up item at position
void tile_map::remove_item_at( int positionX, int positionY){
  item *tempItem = item_at( positionX, positionY);
  if( tempItem != NULL)
    remove_item( tempItem);
}


// Update tile map
void tile_map::update(){
  timer++;
  if(timer >= 120){
    timer = 0;

    for( unsigned int i = 0; i < map_items.size(); i++){
      // Chicken eggs
      tile *backgroundTile = tile_at( map_items.at(i) -> x, map_items.at(i) -> y, BACKGROUND);
      if( map_items.at(i) -> itemPtr -> getID() == 6 && random(0,16) == 1 && backgroundTile != NULL && backgroundTile -> getID() == 59){
        int rand_1 = 16 * random( -1, 1);
        int rand_2 = 16 * random( -1, 1);
        if( !item_at(map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2) && !solid_at( map_items.at(i) -> x + rand_1,map_items.at(i) -> y + rand_2)){
          place_item( new item( 7), map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2);
          play_sample(egg,100,125,1000,0);
        }
      }
    }

    // Run tickers
    for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
      map_tiles_foreground.at(i) -> run_tick();
    }
    for( unsigned int i = 0; i < map_tiles.size(); i++){
      map_tiles.at(i) -> run_tick();
    }

    for( unsigned int i = 0; i < map_tiles.size(); i++){
      // Check crops
      if( map_tiles.at(i) -> requirements_met == true){
        // Berries
        if( map_tiles.at(i) -> getID() == 30 || map_tiles.at(i) -> getID() == 31){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }
        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 32){
          place_item( new item(9), map_tiles.at(i) -> x, map_tiles.at(i) -> y);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Tomatoes
        else if( map_tiles.at(i) -> getID() == 33 || map_tiles.at(i) -> getID() == 34){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }
        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 35){
          place_item( new item(11), map_tiles.at(i) -> x, map_tiles.at(i) -> y);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Tomatoes
        else if( map_tiles.at(i) -> getID() == 36 || map_tiles.at(i) -> getID() == 37){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }
        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 38){
          place_item( new item(13), map_tiles.at(i) -> x, map_tiles.at(i) -> y);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Lavender
        else if( map_tiles.at(i) -> getID() == 39 || map_tiles.at(i) -> getID() == 40){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }

        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 41){
          place_item( new item(15), map_tiles.at(i) -> x, map_tiles.at(i) -> y);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Back to dirt
        else if( map_tiles.at(i) -> getID() == 18){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Back to grass
        else if( map_tiles.at(i) -> getID() == 2){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 0, false);
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
      tempMapForeground[i][t] = 0;

      // Dense grass
      if( random( 0, 10) == 0){
        tempMapForeground[i][t] = 4;
      }
      // Trees
      else if( random( 0, 30) == 0){
        tempMapForeground[i][t] = 5;
      }
      // Bushes
      else if( random( 0, 20) == 0){
        tempMapForeground[i][t] = 6;
      }
    }
  }

  // Grow Grass
  for( int i = 0; i < MAP_WIDTH; i++){
    // Who the frick uses t as an iterator?
    // Allan, that's who!
    for( int t = 0; t < MAP_HEIGHT; t++){
      if( tempMapForeground[i][t] == 4){
        if( i > 0 && random( 0, 2) == 0 && tempMapForeground[i][t]==0){
          tempMapForeground[i - 1][t] = 4;
        }
        if( i < MAP_WIDTH - 1 && random( 0, 2) == 0 && tempMapForeground[i][t]==0){
          tempMapForeground[i + 1][t] = 4;
        }
        if( t > 0 && random( 0, 2) == 0 && tempMapForeground[i][t]==0){
          tempMapForeground[i][t - 1] = 4;
        }
        if( t < MAP_HEIGHT - 1 && random( 0, 2) == 0 && tempMapForeground[i][t]==0){
          tempMapForeground[i][t + 1] = 4;
        }
      }
    }
  }

  // Spread Trees
  for( int j = 0; j < 5; j++){
    for( int i = 0; i < MAP_WIDTH; i++){
      for( int t = 0; t < MAP_HEIGHT; t++){
        if( tempMapForeground[i][t] == 5){
          if( i > 0 && random( 0, 5) == 0  && tempMapForeground[i - 1][t] == 0){
            tempMapForeground[i - 1][t] = 5;
          }
          if( i < MAP_WIDTH - 1 && random( 0, 5) == 0 && tempMapForeground[i + 1][t] == 0){
            tempMapForeground[i + 1][t] = 5;
          }
          if( t > 0 && random( 0, 5) == 0 && tempMapForeground[i][t - 1] == 0){
            tempMapForeground[i][t - 1] = 5;
          }
          if( t < MAP_HEIGHT - 1 && random( 0, 5) == 0 && tempMapForeground[i][t + 1] == 0){
            tempMapForeground[i][t + 1] = 5;
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
      if( backgroundTile && backgroundTile -> getID() == 0 && !foregroundTile){
        if( tempMapForeground[i][t] != 0){
          tile* newTile2 = new tile( i * 16, t * 16, tempMapForeground[i][t]);
          map_tiles_foreground.push_back( newTile2);
        }
      }
    }
  }

  // Place hoe ( 1)
  place_item( new item(1), 17 * 16, 5 * 16);

  // Place chickens (4)
  int placed = 0;
  while( placed < 3){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    tile *foregroundTile = tile_at( random_x, random_y, FOREGROUND);
    if( foregroundTile && foregroundTile -> getID() == 4){
      place_item( new item( 6), random_x, random_y);
      placed += 1;
    }
  }

  placed = 0;
  while( placed < 35){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    if(!solid_at( random_x, random_y)){
      place_item( new item( 16), random_x, random_y );
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

    for( int t = 0; t < MAP_HEIGHT; t++){
      for (int i = 0; i < MAP_WIDTH; i++){
        int newTileType;
        read >> newTileType;
        std::cout << newTileType;
        // Set tile type
        tile* newTile = new tile( i * 16, t * 16, newTileType);
        map_tiles.push_back( newTile);
      }
    }
    read.close();

    fileLoad = fileName + "_back.txt";
    std::ifstream read2(fileLoad.c_str());

    for( int t = 0; t < MAP_HEIGHT; t++){
      for (int i = 0; i < MAP_WIDTH; i++){
        int newTileType;
        read2 >> newTileType;
        std::cout << newTileType;
        // Set tile type
        if( newTileType != 0){
          tile* newTile = new tile( i * 16, t * 16, newTileType);
          map_tiles_foreground.push_back( newTile);
        }
      }
    }
    read2.close();
  }
  std::cout << "\n\n";
}
