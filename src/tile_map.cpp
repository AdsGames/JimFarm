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

bool comparePtrToNode(tile *a, tile *b) {
  return (*a < *b);
}

/*************
 * MAP ITEMS *
 *************/
map_item::~map_item() {
  delete itemPtr;
}

map_item::map_item (int x, int y, item *itemPtr) {
  this -> x = x;
  this -> y = y;
  this -> itemPtr = itemPtr;
}

void map_item::draw (BITMAP *tempBuffer) {
  itemPtr -> draw (x, y, tempBuffer);
}



/************
 * TILE MAP *
 ************/
tile_map::tile_map() {
  x = 128;
  y = 0;

  ticks = 0;
  map_buffer = NULL;

  map_messages = new messenger (1, false, -4);
}


// Ticker
tile_map::~tile_map() {}
volatile int tile_map::ticks = 0;
void tile_map::tick_counter() {
  ticks++;
}
END_OF_FUNCTION(tick_counter)

// Init ticker to 20 ticks per second
void tile_map::init_ticker() {
  LOCK_VARIABLE (ticks);
  LOCK_FUNCTION (tick_counter);

  install_int_ex (tick_counter, BPS_TO_TIMER(20));
}

/*
 * IMAGES
 */
// Draw bottom tiles
void tile_map::draw (BITMAP *tempBuffer) {
  // Draw tiles
  for (unsigned int i = 0; i < map_tiles.size(); i++)
    map_tiles.at(i) -> draw (map_buffer);

  blit (map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

// Draw foreground tiles
void tile_map::drawForeground (BITMAP *tempBuffer) {
  rectfill (map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol (255, 0, 255));

  // Draw items on map
  for (unsigned int i = 0; i < map_items.size(); i++)
    map_items.at(i) -> draw (map_buffer);

  // Draw foreground
  for (unsigned int i = 0; i < map_tiles_foreground.size(); i++)
    map_tiles_foreground.at(i) -> draw (map_buffer);

  masked_blit (map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  // Message system
  map_messages -> draw (tempBuffer, 5, 145);
}

// Load images
void tile_map::load_images() {
  tile_type_manager::sprite_sheet_tiles = load_bitmap_ex ("images/tiles.png");
  tile_type_manager::sprite_sheet_items = load_bitmap_ex ("images/items.png");

  std::cout << "Loading data/tiles.xml \n";
  if (tile_type_manager::load_tiles ("data/tiles.xml"))
    abort_on_error ("Could not load data/tiles.xml");

  std::cout << "Loading data/items.xml \n";
  if (tile_type_manager::load_items ("data/items.xml"))
    abort_on_error ("Could not load data/items.xml");

  std::cout << "Loading data/sounds.xml \n";
  if (sound_manager::load ("data/sounds.xml"))
    abort_on_error ("Could not load data/sounds.xml");
}


/*
 * TILES
 */
// Get tile at position
tile *tile_map::tile_at (int positionX, int positionY, bool foreground) {
  std::vector<tile*> *layer = &map_tiles;
  if (foreground)
    layer = &map_tiles_foreground;

  for (unsigned int i = 0; i < layer -> size(); i++)
    if ((layer -> at(i) -> getX() <= positionX) && (layer -> at(i) -> getX() + layer -> at(i) -> getWidth()  > positionX) &&
        (layer -> at(i) -> getY() - layer -> at(i) -> getHeight() < positionY) && (layer -> at(i) -> getY() >= positionY))
      return layer -> at(i);

  return NULL;
}

// Place tile on map (world gen)
void tile_map::place_tile (tile* newTile, bool foreground) {
  if (newTile) {
    if (foreground)
      map_tiles_foreground.push_back (newTile);
    else{
      map_tiles.push_back (newTile);
    }
  }
}

// Place tile on map if there isnt one otherwise it deletes it (world gen)
bool tile_map::place_tile_safe (tile* newTile, bool foreground, int opposite_layer_id) {
  tile *layerTile = tile_at (newTile -> getX(), newTile -> getY(), foreground);
  tile *notLayerTile = tile_at (newTile -> getX(), newTile -> getY(), !foreground);

  if (opposite_layer_id == -1 || (notLayerTile && notLayerTile -> getID() == opposite_layer_id)) {
    if (newTile && !layerTile) {
      if (foreground)
        map_tiles_foreground.push_back (newTile);
      else
        map_tiles.push_back (newTile);
      return true;
    }
  }
  delete newTile;
  return false;
}

// Replace tile on map
void tile_map::replace_tile (tile *oldTile, int newID, bool foreground) {
  if (oldTile) {
    remove_tile (oldTile, foreground);
    tile *newTile = new tile (newID, oldTile -> getX(), oldTile -> getY());
    place_tile (newTile, foreground);

    // Update bitmasks
    update_bitmask_surround(newTile);
  }
}

// Remove tile from map
void tile_map::remove_tile (tile* newTile, bool foreground) {
  if (newTile) {
    std::vector<tile*> *layer = &map_tiles;
    if (foreground)
      layer = &map_tiles_foreground;

    for (unsigned int i = 0; i < layer -> size(); i++) {
      if (layer -> at(i) == newTile) {
        tile *tempPtr = layer -> at(i);
        layer -> erase (layer -> begin() + i);
        delete tempPtr;
        break;
      }
    }
  }
}

// Check for solid tile
bool tile_map::solid_at (int positionX, int positionY) {
  if (tile_at (positionX, positionY, true))
    return tile_at (positionX, positionY, true) -> isSolid();
  return false;
}

/*
 * ITEMS
 */
// Get item at position
item *tile_map::item_at (int positionX, int positionY) {
  for (unsigned int i = 0; i < map_items.size(); i++)
    if (map_items.at(i) -> x == positionX && map_items.at(i) -> y == positionY)
      return map_items.at(i) -> itemPtr;
  return NULL;
}

// Place item on map
void tile_map::place_item (item* newItem, int x, int y) {
  if (newItem) {
    map_item *newMapItem = new map_item (x, y, newItem);
    map_items.push_back (newMapItem);
  }
}

// Remove item from map
void tile_map::remove_item (item *newItem) {
  if (newItem != NULL) {
    for (unsigned int i = 0; i < map_items.size(); i++) {
      if (map_items.at(i) -> itemPtr == newItem) {
        map_items.erase (map_items.begin() + i);
        break;
      }
    }
  }
}

/*
 * MAP
 */
// Interact with
void tile_map::interact (int inter_x, int inter_y, item *inHand) {
  tile *foregroundTile = tile_at (inter_x, inter_y, FOREGROUND);
  tile *backgroundTile = tile_at (inter_x, inter_y, BACKGROUND);

  // Hoe
  if (inHand -> getID() == ITEM_HOE) {
    if (backgroundTile && !foregroundTile) {
      if (backgroundTile -> getID() == TILE_GRASS) {
        replace_tile (backgroundTile, TILE_PLOWED_SOIL, LAYER_BACKGROUND);
        sound_manager::play (SOUND_HOE);
      }
      else if (backgroundTile -> getID() == TILE_SOIL) {
        replace_tile (backgroundTile, TILE_PLOWED_SOIL, LAYER_BACKGROUND);
        sound_manager::play (SOUND_HOE);
      }
      else {
        map_messages -> push_message ("You can't hoe that");
      }
    }
    else {
      map_messages -> push_message ("You can't hoe there");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Scythe
  else if (inHand -> getID() == ITEM_SCYTHE) {
    if (foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS) {
      remove_tile (foregroundTile, LAYER_FOREGROUND);
      sound_manager::play (SOUND_SCYTHE);
    }
    else {
      map_messages -> push_message ("You can't cut there");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand -> getID() == ITEM_BERRY_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new tile (TILE_BERRY, inter_x, inter_y), LAYER_FOREGROUND);
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand -> getID() == ITEM_TOMATO_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new tile (TILE_TOMATO, inter_x, inter_y), LAYER_FOREGROUND);
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand -> getID() == ITEM_CARROT_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new tile (TILE_CARROT, inter_x, inter_y), LAYER_FOREGROUND);
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand -> getID() == ITEM_LAVENDER_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new tile (TILE_LAVENDER, inter_x, inter_y), LAYER_FOREGROUND);
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand -> getID() == ITEM_WATERING_CAN) {
    if (backgroundTile && backgroundTile -> getID() == TILE_WELL_PATH) {
      map_messages -> push_message ("Watering can filled");
      inHand -> setMeta(8);
      sound_manager::play (SOUND_WATER_FILL);
    }
    else if (inHand -> getMeta() > 0) {
      inHand -> changeMeta(-1);
      map_messages -> push_message("Watered");
      sound_manager::play (SOUND_WATER_POUR);
    }
    else{
      map_messages -> push_message("Out of water");
      sound_manager::play (SOUND_ERROR);
    }
  }
  // Axe
  else if (inHand -> getID() == ITEM_AXE) {
    if (foregroundTile && foregroundTile  -> getID() == TILE_TREE) {
      replace_tile (foregroundTile, TILE_STUMP, LAYER_FOREGROUND);
      sound_manager::play (SOUND_AXE);
    }
    else{
      map_messages -> push_message ("You can't chop that down");
      sound_manager::play (SOUND_ERROR);
    }

    if (backgroundTile && backgroundTile  -> getID() == TILE_GRASS) {
      replace_tile (backgroundTile, TILE_PATH, LAYER_BACKGROUND);
      sound_manager::play (SOUND_AXE);
    }
  }
  // Shovel
  else if (inHand -> getID() == ITEM_SHOVEL) {
    if (foregroundTile && (foregroundTile  -> getID() == TILE_BUSH || foregroundTile  -> getID() == TILE_STUMP)) {
      remove_tile (foregroundTile, LAYER_FOREGROUND);
      sound_manager::play (SOUND_SHOVEL);
    }
    else if ( backgroundTile && backgroundTile -> getID() == TILE_GRASS && foregroundTile) {
      replace_tile (backgroundTile, TILE_SOIL, LAYER_BACKGROUND);
      sound_manager::play (SOUND_SHOVEL);
    }
    else{
      map_messages -> push_message ("You can't dig that up");
      sound_manager::play (SOUND_ERROR);
    }
  }
}

// Update tile map
void tile_map::update() {
  //Reload map (used to test memory leaks)
  if (key[KEY_R]) {
    clear_map();
    generate_map();
  }

  if (ticks >= 1) {
    ticks = 0;

    // Run tickers for itens
    for (unsigned int i = 0; i < map_items.size(); i++) {
      // Current item
      item *current = map_items.at(i) -> itemPtr;

      // Tile near item
      tile *foregroundTile = tile_at (map_items.at(i) -> x, map_items.at(i) -> y, FOREGROUND);
      tile *backgroundTile = tile_at (map_items.at(i) -> x, map_items.at(i) -> y, BACKGROUND);

      // Chicken eggs
      if (current -> getID() == ITEM_CHICKEN) {
        if (backgroundTile && backgroundTile -> getID() == TILE_PATCHY_GRASS) {
          if (!random(0,16)) {
            int rand_1 = 16 * random (-1, 1);
            int rand_2 = 16 * random (-1, 1);
            if (!item_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2) &&
                !solid_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2)) {
              place_item (new item (ITEM_EGG), map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2);
              sound_manager::play (SOUND_EGG);
            }
          }
        }
      }
    }

    // Run tickers for tiles
    for (unsigned int i = 0; i < map_tiles.size(); i++) {
      // Current tile
      tile *current = map_tiles.at(i);

      // Animations
      if (current -> getID() == TILE_WATER) {
        current -> changeMeta(8);
      }
    }

    // Foreground tiles
    for (unsigned int i = 0; i < map_tiles_foreground.size(); i++) {
      // Current tile
      tile *current = map_tiles_foreground.at(i);
      tile *backgroundTile = tile_at (current -> getX(), current -> getY(), BACKGROUND);

      // Berries
      if (current -> getID() == TILE_BERRY) {
        // Grow a bit
        if (true)
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new item(ITEM_BERRY), current -> getX(), current -> getY());
          remove_tile (current, LAYER_FOREGROUND);
          if (backgroundTile)
            replace_tile (backgroundTile, TILE_GRASS, LAYER_BACKGROUND);
        }
      }
      // Tomatos
      else if (current -> getID() == TILE_TOMATO) {
        // Grow a bit
        if (!random (0, 2))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new item(ITEM_TOMATO), current -> getX(), current -> getY());
          remove_tile (current, LAYER_FOREGROUND);
          if (backgroundTile)
            replace_tile (backgroundTile, TILE_GRASS, LAYER_BACKGROUND);
        }
      }
      // Carrots
      else if (current -> getID() == TILE_CARROT) {
        // Grow a bit
        if (!random (0, 5))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new item(ITEM_CARROT), current -> getX(), current -> getY());
          remove_tile (current, LAYER_FOREGROUND);
          if (backgroundTile)
            replace_tile (backgroundTile, TILE_GRASS, LAYER_BACKGROUND);
        }
      }
      // Lavender
      else if (current -> getID() == TILE_LAVENDER) {
        // Grow a bit
        if (!random (0, 10))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new item(ITEM_LAVENDER), current -> getX(), current -> getY());
          remove_tile (current, LAYER_FOREGROUND);
          if (backgroundTile)
            replace_tile (backgroundTile, TILE_GRASS, LAYER_BACKGROUND);
        }
      }
      // Plowed soil to grass
      /*else if (current -> getID() == TILE_PLOWED_SOIL) {
        if (!random (0, 1000))
          replace_tile (current, TILE_GRASS, LAYER_BACKGROUND);
      }*/
    }
  }
}

// Scroll
void tile_map::scroll (int player_x, int player_y) {
  // Scroll x
  if ((player_x + 64) - x > VIEWPORT_WIDTH && x + VIEWPORT_WIDTH < MAP_WIDTH * 16)
    x += 2;
  else if ((player_x - 64) - x < 0 && x > 0)
    x -= 2;

  // Scroll y
  if ((player_y + 48) - y > VIEWPORT_HEIGHT && y + VIEWPORT_HEIGHT < MAP_HEIGHT * 16)
    y += 2;
  else if ((player_y - 48) - y < 0 && y > 0)
    y -= 2;
}

// Update bitmask
void tile_map::update_bitmask (tile *newTile, bool layer) {
  if (newTile && newTile -> needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, layer);
      if (current && current -> getID() == newTile -> getID())
        mask += pow (2, i);
    }

    newTile -> setMeta (mask);
  }
}

// Update bitmask (and neighbours)
void tile_map::update_bitmask_surround (tile *newTile, bool layer) {
  if (newTile) {
    update_bitmask (newTile);
    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, layer);
      if (current)
        update_bitmask (current);
    }
  }
}

// Generate map
void tile_map::generate_map() {
  // Base map
  load_map ("data/map");

  // Place grass
  int placed = 0;
  while (placed < 10) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    tile *backgroundTile = tile_at (random_x, random_y, BACKGROUND);
    placed += place_tile_safe (new tile (TILE_DENSE_GRASS, random_x, random_y, random(0, 5)), FOREGROUND, TILE_GRASS);
  }

  // Grow Grass
  placed = 0;
  while (placed < 100) {
    for (unsigned int t = 0; t < map_tiles_foreground.size(); t++) {
      if (map_tiles_foreground.at(t) -> getID() == TILE_DENSE_GRASS && !random(0, 10)) {
        tile *current = map_tiles_foreground.at(t);
        placed += place_tile_safe (new tile(TILE_DENSE_GRASS, current -> getX() + random(-1, 1) * 16,
                                            current -> getY() + random(-1, 1) * 16, current -> getMeta()),
                                  FOREGROUND, TILE_GRASS);
      }
    }
  }

  // Place trees
  placed = 0;
  while (placed < 20) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    tile *backgroundTile = tile_at (random_x, random_y, BACKGROUND);
    placed += place_tile_safe (new tile (TILE_TREE, random_x, random_y), FOREGROUND, TILE_GRASS);
  }

  // Grow trees
  placed = 0;
  while (placed < 40)
    for (unsigned int t = 0; t < map_tiles_foreground.size(); t++)
      if (map_tiles_foreground.at(t) -> getID() == TILE_TREE && !random(0, 10))
        placed += place_tile_safe (new tile(TILE_TREE, map_tiles_foreground.at(t) -> getX() + random(-1, 1) * 16, map_tiles_foreground.at(t) -> getY() + random(-1, 1) * 16), FOREGROUND, TILE_GRASS);

  // Place chickens
  placed = 0;
  while (placed < 3) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    tile *foregroundTile = tile_at (random_x, random_y, FOREGROUND);
    if (foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS) {
      place_item (new item (ITEM_CHICKEN), random_x, random_y);
      placed ++;
    }
  }

  // Place sticks
  placed = 0;
  while (placed < 10) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    if(!solid_at (random_x, random_y)) {
      place_item (new item (ITEM_STICK), random_x, random_y );
      placed ++;
    }
  }

  // Place items
  place_item (new item(ITEM_HOE), 17 * 16, 5 * 16);
  place_item (new item(ITEM_CARROT_SEED), 18 * 16, 5 * 16);
  place_item (new item(ITEM_LAVENDER_SEED), 19 * 16, 5 * 16);
  place_item (new item(ITEM_BERRY_SEED), 20 * 16, 5 * 16);
  place_item (new item(ITEM_TOMATO_SEED), 21 * 16, 5 * 16);
  place_item (new item(ITEM_WATERING_CAN), 17 * 16, 6 * 16);
  place_item (new item(ITEM_AXE), 18 * 16, 6 * 16);
  place_item (new item(ITEM_SCYTHE), 19 * 16, 6 * 16);

  // Update masks
  for (unsigned int i = 0; i < map_tiles.size(); i++)
    update_bitmask (map_tiles.at(i));
  for (unsigned int i = 0; i < map_tiles_foreground.size(); i++)
    update_bitmask (map_tiles_foreground.at(i), LAYER_FOREGROUND);

  // SORT IT OUT!
  std::sort (map_tiles_foreground.begin(), map_tiles_foreground.end(), comparePtrToNode);

  // Create map buffer (and destroy the old one)
  destroy_bitmap(map_buffer);
  map_buffer = create_bitmap (MAP_WIDTH * 16, MAP_HEIGHT * 16);
}

// Manually load new file
void tile_map::load_map (std::string fileName) {
  //Change size
  std::string fileLoad = fileName + ".txt";
  std::ifstream findSize(fileLoad.c_str());

  MAP_WIDTH = 0;
  MAP_HEIGHT = 0;

  int data;
  while (findSize >> data) {
    if(MAP_HEIGHT == 0)
      MAP_WIDTH++;
    if(findSize.peek() == '\n')
      MAP_HEIGHT++;
  }

  //Setup Map
  if(fileName != "blank") {
    fileLoad = fileName + ".txt";
    std::ifstream read(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for (int t = 0; t < MAP_HEIGHT; t++) {
      for (int i = 0; i < MAP_WIDTH; i++) {
        int newTileType;
        read >> newTileType;
        tile* newTile = new tile (newTileType, i * 16, t * 16);
        map_tiles.push_back (newTile);
      }
    }
    read.close();

    fileLoad = fileName + "_back.txt";
    std::ifstream read2(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for (int t = 0; t < MAP_HEIGHT; t++) {
      for (int i = 0; i < MAP_WIDTH; i++) {
        int newTileType;
        read2 >> newTileType;
        if (newTileType != TILE_NULL) {
          tile* newTile = new tile (newTileType, i * 16, t * 16);
          map_tiles_foreground.push_back (newTile);
        }
      }
    }
    read2.close();
  }
}

// Clear map
void tile_map::clear_map() {
  for (std::vector<tile*>::iterator i = map_tiles.begin(); i != map_tiles.end(); ++i)
    delete *i;
  for (std::vector<tile*>::iterator i = map_tiles_foreground.begin(); i != map_tiles_foreground.end(); ++i)
    delete *i;
  for (std::vector<map_item*>::iterator i = map_items.begin(); i != map_items.end(); ++i)
    delete *i;

  map_tiles.clear();
  map_tiles_foreground.clear();
  map_items.clear();
}
