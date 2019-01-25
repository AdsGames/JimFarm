#include "World.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>

#include "Tools.h"
#include "TileTypeManager.h"

#include "tile_defs.h"
#include "item_defs.h"
#include "sound_defs.h"

#include "SoundManager.h"
#include "KeyListener.h"

bool comparePtrToNode(Tile *a, Tile *b) {
  return (*a < *b);
}

bool sortDrawableByZ(Sprite *A, Sprite *B) {
  return (*A < *B);
}

/************
 * TILE MAP *
 ************/
World::World() {
  x = 128;
  y = 0;

  ticks = 0;
  map_buffer = NULL;

  map_messages = new Messenger (1, false, -4);

  VIEWPORT_ZOOM = 1.0f;

  should_sort = true;
}


// Ticker
World::~World() {}

volatile int World::ticks = 0;

void World::tick_counter() {
  ticks++;
}
END_OF_FUNCTION(tick_counter)

// Init ticker to 20 ticks per second
void World::init_ticker() {
  LOCK_VARIABLE (ticks);
  LOCK_FUNCTION (tick_counter);

  install_int_ex (tick_counter, BPS_TO_TIMER(20));
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw (BITMAP *tempBuffer) {
  // Draw tiles
  rectfill (map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol (255, 0, 0));

  // Drawable
  for (unsigned int i = 0; i < drawable.size(); i++) {
    drawable.at(i) -> draw (map_buffer, x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM, y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);
  }

  stretch_blit (map_buffer, tempBuffer, 0, 0, VIEWPORT_WIDTH / VIEWPORT_ZOOM, VIEWPORT_HEIGHT / VIEWPORT_ZOOM, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  // Message system
  map_messages -> draw (tempBuffer, 5, 145);
}

// Load images
void World::load_images() {
  TileTypeManager::sprite_sheet_tiles = load_bitmap_ex ("images/tiles.png");
  TileTypeManager::sprite_sheet_items = load_bitmap_ex ("images/items.png");

  std::cout << "Loading data/tiles.xml \n";
  if (TileTypeManager::load_tiles ("data/tiles.xml"))
    abort_on_error ("Could not load data/tiles.xml");

  std::cout << "Loading data/items.xml \n";
  if (TileTypeManager::load_items ("data/items.xml"))
    abort_on_error ("Could not load data/items.xml");

  std::cout << "Loading data/sounds.xml \n";
  if (SoundManager::load ("data/sounds.xml"))
    abort_on_error ("Could not load data/sounds.xml");

  // Create map buffer
  map_buffer = create_bitmap (VIEWPORT_WIDTH * VIEWPORT_MAX_ZOOM, VIEWPORT_HEIGHT * VIEWPORT_MAX_ZOOM);
}

// Add drawable
void World::add_sprite(Sprite* sprite) {
  drawable.push_back(sprite);
  if (should_sort)
    sort_drawables();
}

// Add drawable
void World::remove_sprite(Sprite* sprite) {
  drawable.erase(std::remove(drawable.begin(), drawable.end(), sprite), drawable.end());
}

// Sort drawables
void World::sort_drawables() {
  std::sort(drawable.begin(), drawable.end(), sortDrawableByZ);
}


/*
 * TILES
 */
// Get tile at position
Tile *World::tile_at (int positionX, int positionY, int layer) {
  for (unsigned int i = 0; i < map_tiles.size(); i++)
    if ( map_tiles.at(i) -> getZ() == layer &&
        (map_tiles.at(i) -> getX() <= positionX) &&
        (map_tiles.at(i) -> getX() + map_tiles.at(i) -> getWidth() > positionX) &&
        (map_tiles.at(i) -> getY() - map_tiles.at(i) -> getHeight() < positionY) &&
        (map_tiles.at(i) -> getY() >= positionY))
      return map_tiles.at(i);

  return NULL;
}

// Place tile on map (world gen)
void World::place_tile (Tile* newTile) {
  if (newTile) {
    map_tiles.push_back (newTile);
    add_sprite(newTile);
  }
}

// Place tile on map if there isnt one otherwise it deletes it (world gen)
bool World::place_tile_safe (Tile* newTile, int opposite_layer_id) {
  Tile *layerTile = tile_at (newTile -> getX(), newTile -> getY(), newTile -> getZ());
  Tile *notLayerTile = tile_at (newTile -> getX(), newTile -> getY(), LAYER_BACKGROUND);

  if (opposite_layer_id == -1 || (notLayerTile && notLayerTile -> getID() == opposite_layer_id)) {
    if (!layerTile) {
      place_tile (newTile);
      return true;
    }
  }
  delete newTile;
  return false;
}

// Replace tile on map
void World::replace_tile (Tile *oldTile, Tile *newTile) {
  if (oldTile) {
    remove_tile (oldTile);
    place_tile (newTile);

    // Update bitmasks
    update_bitmask_surround(newTile);
  }
}

// Remove tile from map
void World::remove_tile (Tile* newTile) {
  if (newTile) {
    for (unsigned int i = 0; i < map_tiles.size(); i++) {
      if (map_tiles.at(i) == newTile) {
        remove_sprite(map_tiles.at(i));
        Tile *tempPtr = map_tiles.at(i);
        map_tiles.erase (map_tiles.begin() + i);
        delete tempPtr;
        break;
      }
    }
  }
}

// Check for solid tile
bool World::solid_at (int positionX, int positionY) {
  if (tile_at (positionX, positionY, LAYER_FOREGROUND))
    return tile_at (positionX, positionY, LAYER_FOREGROUND) -> isSolid();
  return false;
}

/*
 * ITEMS
 */
// Get item at position
Item *World::item_at (int positionX, int positionY) {
  for (unsigned int i = 0; i < map_items.size(); i++)
    if (map_items.at(i) -> x == positionX && map_items.at(i) -> y == positionY)
      return map_items.at(i) -> itemPtr;
  return NULL;
}

// Place item on map
void World::place_item (Item* newItem, int x, int y) {
  if (newItem) {
    MapItem *newMapItem = new MapItem (x, y, newItem);
    map_items.push_back (newMapItem);
    add_sprite(newMapItem);
  }
}

// Remove item from map
void World::remove_item (Item *newItem) {
  if (newItem != NULL) {
    for (unsigned int i = 0; i < map_items.size(); i++) {
      if (map_items.at(i) -> itemPtr == newItem) {
        remove_sprite(map_items.at(i));
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
void World::interact (int inter_x, int inter_y, Item *inHand) {
  Tile *foregroundTile = tile_at (inter_x, inter_y, LAYER_FOREGROUND);
  Tile *backgroundTile = tile_at (inter_x, inter_y, LAYER_BACKGROUND);

  // Hoe
  if (inHand -> getID() == ITEM_HOE) {
    if (backgroundTile && !foregroundTile) {
      if (backgroundTile -> getID() == TILE_GRASS) {
        replace_tile (backgroundTile, new Tile(TILE_PLOWED_SOIL, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        SoundManager::play (SOUND_HOE);
      }
      else if (backgroundTile -> getID() == TILE_SOIL) {
        replace_tile (backgroundTile, new Tile(TILE_PLOWED_SOIL, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        SoundManager::play (SOUND_HOE);
      }
      else {
        map_messages -> push_message ("You can't hoe that");
      }
    }
    else {
      map_messages -> push_message ("You can't hoe there");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Scythe
  else if (inHand -> getID() == ITEM_SCYTHE) {
    if (foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS) {
      remove_tile (foregroundTile);
      place_item (new Item(ITEM_HAY, 0), backgroundTile -> getX(), backgroundTile -> getY());
      SoundManager::play (SOUND_SCYTHE);
    }
    else {
      map_messages -> push_message ("You can't cut there");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand -> getID() == ITEM_BERRY_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new Tile (TILE_BERRY, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand -> getID() == ITEM_TOMATO_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new Tile (TILE_TOMATO, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand -> getID() == ITEM_CARROT_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new Tile (TILE_CARROT, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand -> getID() == ITEM_LAVENDER_SEED) {
    if (backgroundTile && backgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      place_tile (new Tile (TILE_LAVENDER, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand -> getID() == ITEM_WATERING_CAN) {
    if (backgroundTile && backgroundTile -> getID() == TILE_WELL_PATH) {
      map_messages -> push_message ("Watering can filled");
      inHand -> setMeta(8);
      SoundManager::play (SOUND_WATER_FILL);
    }
    else if (inHand -> getMeta() > 0) {
      inHand -> changeMeta(-1);
      map_messages -> push_message("Watered");
      SoundManager::play (SOUND_WATER_POUR);
    }
    else{
      map_messages -> push_message("Out of water");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Axe
  else if (inHand -> getID() == ITEM_AXE) {
    if (foregroundTile && foregroundTile  -> getID() == TILE_TREE) {
      replace_tile (foregroundTile, new Tile(TILE_STUMP, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
      place_item (new Item(ITEM_STICK, 0), backgroundTile -> getX(), backgroundTile -> getY());
      place_item (new Item(ITEM_STICK, 0), backgroundTile -> getX(), backgroundTile -> getY());
      place_item (new Item(ITEM_WOOD, 0), backgroundTile -> getX(), backgroundTile -> getY());
      SoundManager::play (SOUND_AXE);
    }
    else{
      map_messages -> push_message ("You can't chop that down");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Shovel
  else if (inHand -> getID() == ITEM_SHOVEL) {
    if (foregroundTile && (foregroundTile  -> getID() == TILE_BUSH || foregroundTile  -> getID() == TILE_STUMP)) {
      remove_tile (foregroundTile);
      SoundManager::play (SOUND_SHOVEL);
    }
    else if ( backgroundTile && backgroundTile -> getID() == TILE_GRASS && !foregroundTile) {
      replace_tile (backgroundTile, new Tile(TILE_PATH, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
      SoundManager::play (SOUND_SHOVEL);
    }
    else{
      map_messages -> push_message ("You can't dig that up");
      SoundManager::play (SOUND_ERROR);
    }
  }
}

// Update tile map
void World::update() {
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
      Item *current = map_items.at(i) -> itemPtr;

      // Tile near item
      //Tile *foregroundTile = tile_at (map_items.at(i) -> x, map_items.at(i) -> y, FOREGROUND);
      Tile *backgroundTile = tile_at (map_items.at(i) -> x, map_items.at(i) -> y, LAYER_BACKGROUND);

      // Chicken eggs
      if (current -> getID() == ITEM_CHICKEN) {
        if (backgroundTile && backgroundTile -> getID() == TILE_PATCHY_GRASS) {
          if (!random(0,32)) {
            int rand_1 = 16 * random (-1, 1);
            int rand_2 = 16 * random (-1, 1);
            if (!item_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2) &&
                !solid_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2)) {
              place_item (new Item (ITEM_EGG), map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2);
              SoundManager::play (SOUND_EGG);
            }
          }
        }
      }
    }

    // Foreground tiles
    for (unsigned int i = 0; i < map_tiles.size(); i++) {
      // Current tile
      Tile *current = map_tiles.at(i);
      Tile *backgroundTile = tile_at (current -> getX(), current -> getY(), LAYER_BACKGROUND);

      // Berries
      if (current -> getID() == TILE_BERRY) {
        // Grow a bit
        if (true)
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new Item(ITEM_BERRY), current -> getX(), current -> getY());
          remove_tile (current);
          if (backgroundTile)
            replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        }
      }
      // Tomatos
      else if (current -> getID() == TILE_TOMATO) {
        // Grow a bit
        if (!random (0, 2))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new Item(ITEM_TOMATO), current -> getX(), current -> getY());
          remove_tile (current);
          if (backgroundTile)
            replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        }
      }
      // Carrots
      else if (current -> getID() == TILE_CARROT) {
        // Grow a bit
        if (!random (0, 5))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new Item(ITEM_CARROT), current -> getX(), current -> getY());
          remove_tile (current);
          if (backgroundTile)
            replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        }
      }
      // Lavender
      else if (current -> getID() == TILE_LAVENDER) {
        // Grow a bit
        if (!random (0, 10))
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new Item(ITEM_LAVENDER), current -> getX(), current -> getY());
          remove_tile (current);
          if (backgroundTile)
            replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        }
      }
      // Plowed soil to grass
      /*else if (current -> getID() == TILE_PLOWED_SOIL) {
        if (!random (0, 1000))
          replace_tile (current, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()), LAYER_BACKGROUND);
      }*/
    }
  }

  // Zooming
  if (KeyListener::keyPressed[KEY_PLUS_PAD] && VIEWPORT_ZOOM < VIEWPORT_MAX_ZOOM) {
    VIEWPORT_ZOOM *= 2.0f;
  }
  if (KeyListener::keyPressed[KEY_MINUS_PAD] && VIEWPORT_ZOOM > VIEWPORT_MIN_ZOOM) {
    VIEWPORT_ZOOM *= 0.5f;
  }
}

// Scroll
void World::scroll (int player_x, int player_y) {
  // Scroll x
  if ((player_x + 64 / VIEWPORT_ZOOM) - x > VIEWPORT_WIDTH / VIEWPORT_ZOOM && x + VIEWPORT_WIDTH / VIEWPORT_ZOOM < MAP_WIDTH * 16)
    x += 2;
  else if ((player_x - 64 / VIEWPORT_ZOOM) - x < 0 && x > 0)
    x -= 2;

  // Scroll y
  if ((player_y + 48 / VIEWPORT_ZOOM) - y > VIEWPORT_HEIGHT / VIEWPORT_ZOOM && y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM < MAP_HEIGHT * 16)
    y += 2;
  else if ((player_y - 48 / VIEWPORT_ZOOM) - y < 0 && y > 0)
    y -= 2;
}

// Update bitmask
void World::update_bitmask (Tile *newTile, bool layer) {
  if (newTile && newTile -> needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, newTile -> getZ());
      if (current && current -> getID() == newTile -> getID())
        mask += pow (2, i);
    }

    newTile -> setMeta (mask);
  }
}

// Update bitmask (and neighbours)
void World::update_bitmask_surround (Tile *newTile, bool layer) {
  if (newTile) {
    update_bitmask (newTile);
    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, layer);
      if (current)
        update_bitmask (current);
    }
  }
}

// Generate map
void World::generate_map() {
  // Base map
  load_map ("data/map");

  // Disable sorting
  should_sort = false;
  const int PLACED_MULTIPLIER = MAP_WIDTH / 16 + MAP_HEIGHT / 16;

  // Grass
  for (int t = 0; t < MAP_HEIGHT; t++) {
    for (int i = 0; i < MAP_WIDTH; i++) {
      Tile* newTile = new Tile (TILE_GRASS, i * 16, t * 16, LAYER_BACKGROUND);
      map_tiles.push_back (newTile);
      add_sprite(newTile);
    }
  }

  // Rivers
  int placed = 0;
  while (placed < (int)(0.25f * (float)PLACED_MULTIPLIER)) {
    int river_x_1 = random(0, MAP_WIDTH - 1);
    int river_y_1 = 0;
    int river_x_2 = random(0, MAP_WIDTH - 1);
    int river_y_2 = MAP_HEIGHT;
    int river_width = random(4,8);

    while (river_y_1 < river_y_2) {
      for (int i = 0; i < river_width; i++) {
        if (river_x_1 + i < MAP_WIDTH) {
          Tile *foregroundTile = tile_at((river_x_1 + i) * 16, river_y_1 * 16, LAYER_FOREGROUND);
          Tile *backgroundTile = tile_at((river_x_1 + i) * 16, river_y_1 * 16, LAYER_BACKGROUND);
          if (!foregroundTile) {
            place_tile(new Tile (TILE_WATER, (river_x_1 + i) * 16, river_y_1 * 16, LAYER_FOREGROUND));

            int underwater_meta = random(0,100);
            if (underwater_meta > 3)
              underwater_meta = 0;

            replace_tile(backgroundTile, new Tile (TILE_UNDERWATER_SOIL, (river_x_1 + i) * 16, river_y_1 * 16, LAYER_BACKGROUND, underwater_meta));
          }
        }
      }

      if (river_x_1 < MAP_WIDTH && river_x_1 > 0 && random(0, 2) == 0) {
        river_x_1 += int(river_x_1 < river_x_2) - int(river_x_1 > river_x_2);
        if (river_x_1 < MAP_WIDTH) {
          Tile *foregroundTile = tile_at(river_x_1 * 16, river_y_1 * 16, LAYER_FOREGROUND);
          Tile *backgroundTile = tile_at(river_x_1 * 16, river_y_1 * 16, LAYER_BACKGROUND);
          if (!foregroundTile) {
            place_tile(new Tile (TILE_WATER, river_x_1 * 16, river_y_1 * 16, LAYER_FOREGROUND));

            int underwater_meta = random(0,100);
            if (underwater_meta > 3)
              underwater_meta = 0;

            replace_tile(backgroundTile, new Tile (TILE_UNDERWATER_SOIL, river_x_1 * 16, river_y_1 * 16, LAYER_BACKGROUND, underwater_meta));
          }
        }
      }
      if (river_y_1 < MAP_HEIGHT)
        river_y_1 ++;
    }
    placed++;
  }

  // Place grass
  placed = 0;
  while (placed < 10 * PLACED_MULTIPLIER) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    //Tile *backgroundTile = tile_at (random_x, random_y, BACKGROUND);
    placed += place_tile_safe (new Tile (TILE_DENSE_GRASS, random_x, random_y, LAYER_FOREGROUND, random(0, 5)), TILE_GRASS);
  }

  // Grow Grass
  placed = 0;
  while (placed < 100 * PLACED_MULTIPLIER) {
    for (unsigned int t = 0; t < map_tiles.size(); t++) {
      if (map_tiles.at(t) -> getID() == TILE_DENSE_GRASS && !random(0, 10)) {
        Tile *current = map_tiles.at(t);
        placed += place_tile_safe (new Tile(TILE_DENSE_GRASS, current -> getX() + random(-1, 1) * 16,
                                            current -> getY() + random(-1, 1) * 16, LAYER_FOREGROUND, current -> getMeta()),
                                            TILE_GRASS);
      }
    }
  }

  // Place trees
  placed = 0;
  while (placed < 20 * PLACED_MULTIPLIER) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    //Tile *backgroundTile = tile_at (random_x, random_y, FOREGROUND);
    placed += place_tile_safe (new Tile (TILE_TREE, random_x, random_y, LAYER_FOREGROUND, random(0,3)), TILE_GRASS);
  }

  // Grow trees
  placed = 0;
  while (placed < 90 * PLACED_MULTIPLIER)
    for (unsigned int t = 0; t < map_tiles.size(); t++)
      if (map_tiles.at(t) -> getID() == TILE_TREE && !random(0, 10))
        placed += place_tile_safe (new Tile(TILE_TREE,
                                            map_tiles.at(t) -> getX() + random(-1, 1) * 16,
                                            map_tiles.at(t) -> getY() + random(-1, 1) * 16,
                                            LAYER_FOREGROUND, random(0,3)),
                                            TILE_GRASS);

  // Place chickens
  placed = 0;
  while (placed < 3 * PLACED_MULTIPLIER) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    Tile *foregroundTile = tile_at (random_x, random_y, LAYER_FOREGROUND);
    if (foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS) {
      place_item (new Item (ITEM_CHICKEN), random_x, random_y);
      placed ++;
    }
  }

  // Place sticks
  placed = 0;
  while (placed < 10 * PLACED_MULTIPLIER) {
    int random_x = random (0, MAP_WIDTH) * 16;
    int random_y = random (0, MAP_HEIGHT) * 16;
    if(!solid_at (random_x, random_y)) {
      place_item (new Item (ITEM_STICK), random_x, random_y );
      placed ++;
    }
  }

  // Place items
  place_item (new Item(ITEM_HOE), 17 * 16, 5 * 16);
  place_item (new Item(ITEM_CARROT_SEED), 18 * 16, 5 * 16);
  place_item (new Item(ITEM_LAVENDER_SEED), 19 * 16, 5 * 16);
  place_item (new Item(ITEM_BERRY_SEED), 20 * 16, 5 * 16);
  place_item (new Item(ITEM_TOMATO_SEED), 21 * 16, 5 * 16);
  place_item (new Item(ITEM_WATERING_CAN), 17 * 16, 6 * 16);
  place_item (new Item(ITEM_AXE), 18 * 16, 6 * 16);
  place_item (new Item(ITEM_SCYTHE), 19 * 16, 6 * 16);
  place_item (new Item(ITEM_SHOVEL), 20 * 16, 6 * 16);

  // Update masks
  for (unsigned int i = 0; i < map_tiles.size(); i++)
    update_bitmask (map_tiles.at(i));

  // Sort
  should_sort = true;
  sort_drawables();
}

// Manually load new file
void World::load_map (std::string fileName) {
  //Change size
  std::string fileLoad = fileName + ".txt";
  std::ifstream findSize(fileLoad.c_str());

  MAP_WIDTH = 64;
  MAP_HEIGHT = 64;

  /*int data;
  while (findSize >> data) {
    if(MAP_HEIGHT == 0)
      MAP_WIDTH++;
    if(findSize.peek() == '\n')
      MAP_HEIGHT++;
  }

  //Setup Map
  if(fileName != "") {
    fileLoad = fileName + ".txt";
    std::ifstream read(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for (int t = 0; t < MAP_HEIGHT; t++) {
      for (int i = 0; i < MAP_WIDTH; i++) {
        int newTileType;
        read >> newTileType;
        Tile* newTile = new Tile (newTileType, i * 16, t * 16);
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
          Tile* newTile = new Tile (newTileType, i * 16, t * 16);
          map_tiles_foreground.push_back (newTile);
        }
      }
    }
    read2.close();
  }*/
}

// Clear map
void World::clear_map() {
  for (std::vector<Tile*>::iterator i = map_tiles.begin(); i != map_tiles.end(); ++i) {
    delete *i;
  }
  for (std::vector<MapItem*>::iterator i = map_items.begin(); i != map_items.end(); ++i) {
    delete *i;
  }

  map_tiles.clear();
  map_items.clear();
  drawable.clear();
}
