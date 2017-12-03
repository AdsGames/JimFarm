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

tile_map::tile_map(){
  x = 128;
  y = 0;
}

tile_map::~tile_map(){}

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

// Draw tiles
void tile_map::draw( BITMAP *tempBuffer){
  // Draw tiles
  for( unsigned int i = 0; i < map_tiles.size(); i++){
    map_tiles.at(i) -> draw( map_buffer);
  }

  blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

// Draw tiles
void tile_map::drawForeground( BITMAP *tempBuffer){
  rectfill( map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol( 255, 0, 255));

  // Draw items
  for( unsigned int i = 0; i < map_items.size(); i++){
    map_items.at(i).draw( map_buffer);
  }

  // Draw foreground
  for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
    map_tiles_foreground.at(i) -> draw( map_buffer);
  }

  masked_blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
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
      if( get_tile_at( i * 16, t * 16, false) == 0 && get_tile_at( i * 16, t * 16, true) == -1){
        if( tempMapForeground[i][t] != 0){
          tile* newTile2 = new tile( i * 16, t * 16, tempMapForeground[i][t]);
          map_tiles_foreground.push_back( newTile2);
        }
      }
    }
  }

  // Place hoe ( 1)
  place_new_item_at( 17 * 16, 5 * 16, 1);

  // Place chickens (4)
  int placed = 0;
  while( placed < 3){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    if(get_tile_at(random_x,random_y,FOREGROUND)==4){
      item newItem( random_x, random_y, 6);
      place_item( newItem);
      placed += 1;
    }
  }

  placed = 0;
  while( placed < 35){
    int random_x = random( 0, MAP_WIDTH) * 16;
    int random_y = random( 0, MAP_HEIGHT) * 16;
    if(!is_solid_at( random_x, random_y)){
      item newItem( random_x, random_y, 16);
      place_item( newItem);
      placed += 1;
    }
  }


  // SORT IT OUT!
  std::sort( map_tiles_foreground.begin(), map_tiles_foreground.end(), comparePtrToNode);

  // Create map buffer
  map_buffer = create_bitmap( MAP_WIDTH * 16, MAP_HEIGHT * 16);
}

// Load images
void tile_map::load_images(){
  tile_type_manager::sprite_sheet_tiles = load_bitmap_ex( "images/tiles.png");
  tile_type_manager::sprite_sheet_items = load_bitmap_ex( "images/items.png");

  tile_type_manager::load("data/tiles.xml", false);
  tile_type_manager::load("data/items.xml", true);

  egg = load_sample_ex("sfx/egg.wav");
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID, bool foreground){
  if( !foreground){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
      if( map_tiles.at(i) -> x == tileX && map_tiles.at(i) -> y == tileY){
        if( newID == -1){
          map_tiles.erase( map_tiles.begin() + i);
        }
        else{
          map_tiles.at(i) -> setID( newID);
          map_tiles.at(i) -> requirements_met = false;
        }
      }
    }
  }
  else{
    for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
      if( map_tiles_foreground.at(i) -> x == tileX && map_tiles_foreground.at(i) -> y == tileY){
        if( newID == -1){
          map_tiles_foreground.erase( map_tiles_foreground.begin() + i);
        }
        else{
          map_tiles_foreground.at(i) -> setID( newID) ;
          map_tiles_foreground.at(i) -> requirements_met = false;
        }
      }
    }
  }
}

// Place item on map
void tile_map::place_item( item newItem){
  map_items.push_back( newItem);
}

// Place tile on map
void tile_map::place_tile( tile* newTile){
  map_tiles.push_back( newTile);
}

// Get tile at position
int tile_map::get_tile_at( int positionX, int positionY, bool foreground){
  if( !foreground){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
      if( map_tiles.at(i) -> x == positionX && map_tiles.at(i) -> y == positionY){
        return map_tiles.at(i) -> getID();
      }
    }
  }
  else{
    for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
      if( map_tiles_foreground.at(i) -> x == positionX && map_tiles_foreground.at(i) -> y == positionY){
        return map_tiles_foreground.at(i) -> getID();
      }
    }
  }
  return -1;
}

// Check for solid tile
bool tile_map::is_solid_at( int positionX, int positionY){
  for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
    if( map_tiles_foreground.at(i) -> x <= positionX && (map_tiles_foreground.at(i) -> y + 16) - ceil(double(map_tiles_foreground.at(i) -> getHeight())/2) * 16 <= positionY &&
      map_tiles_foreground.at(i) -> x + map_tiles_foreground.at(i) -> getWidth() * 16  > positionX &&
      map_tiles_foreground.at(i) -> y + 16 > positionY){
      if( map_tiles_foreground.at(i) -> isSolid()){
        return true;
      }
    }
  }
  return false;
}


// Check if item exists
bool tile_map::is_item_at( int positionX, int positionY){
  for( unsigned int i = 0; i < map_items.size(); i++){
    if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
      return true;
    }
  }
  return false;
}

// Get item at position
item *tile_map::get_item_at( int positionX, int positionY){
  for( unsigned int i = 0; i < map_items.size(); i++){
    if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
      return &map_items.at(i);
    }
  }
  return NULL;
}

void tile_map::update(){
  timer++;
  if(timer >= 120){
    timer = 0;

    for( unsigned int i = 0; i < map_items.size(); i++){
      // Chicken eggs
      if( map_items.at(i).getID() == 6  && random(0,16) == 1 && get_tile_at(map_items.at(i).x,map_items.at(i).y,BACKGROUND) == 59){
        int rand_1 = 16*random(-1,1);
        int rand_2 = 16*random(-1,1);
        if(!is_item_at(map_items.at(i).x+rand_1,map_items.at(i).y+rand_2) && !is_solid_at(map_items.at(i).x+rand_1,map_items.at(i).y+rand_2)){
          item newItem( map_items.at(i).x+rand_1, map_items.at(i).y+rand_2, 7);
          place_item( newItem);
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
          place_new_item_at( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 9);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Tomatoes
        else if( map_tiles.at(i) -> getID() == 33 || map_tiles.at(i) -> getID() == 34){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }
        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 35){
          place_new_item_at( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 11);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Tomatoes
        else if( map_tiles.at(i) -> getID() == 36 || map_tiles.at(i) -> getID() == 37){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }
        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 38){
          place_new_item_at( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 13);
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 2, false);
        }

        // Lavender
        else if( map_tiles.at(i) -> getID() == 39 || map_tiles.at(i) -> getID() == 40){
          replace_tile( map_tiles.at(i) -> x, map_tiles.at(i) -> y, map_tiles.at(i) -> getID() + 1, false);
        }

        // YUM YUM
        else if( map_tiles.at(i) -> getID() == 41){
          place_new_item_at( map_tiles.at(i) -> x, map_tiles.at(i) -> y, 15);
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

// Pick up item at position
void tile_map::remove_item_at( int positionX, int positionY){
  for( unsigned int i = 0; i < map_items.size(); i++){
    if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
      map_items.erase( map_items.begin() + i);
      break;
    }
  }
}

// Create item
void tile_map::place_new_item_at( int newX, int newY, unsigned char newItem){
  item newItemx( newX, newY, newItem);
  place_item( newItemx);
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
