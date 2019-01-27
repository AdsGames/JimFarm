#include "World.h"

#include <iostream>

#include "Tools.h"
#include "TileTypeManager.h"

#include "tile_defs.h"
#include "item_defs.h"
#include "sound_defs.h"

#include "SoundManager.h"
#include "KeyListener.h"

#include "Graphics.h"

bool comparePtrToNode(Tile *a, Tile *b) {
  return (*a < *b);
}

/************
 * TILE MAP *
 ************/
World::World() {
  x = 0;
  y = 0;

  ticks = 0;
  map_buffer = NULL;

  map_messages = new Messenger(1, false, -4);

  VIEWPORT_ZOOM = 1.0f;

  world_map = new TileMap();
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
  // Clear buffer
  rectfill (map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol (255, 0, 0));

  // Drawable
  Graphics::Instance() -> draw (map_buffer, x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM, y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);

  // Draw buffer
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

  world_map -> generate_map();
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
    Graphics::Instance() -> add(newMapItem);
  }
}

// Remove item from map
void World::remove_item (Item *newItem) {
  if (newItem != NULL) {
    for (unsigned int i = 0; i < map_items.size(); i++) {
      if (map_items.at(i) -> itemPtr == newItem) {
        Graphics::Instance() -> remove(map_items.at(i));
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
  Tile *foregroundTile = world_map -> tile_at (inter_x, inter_y, LAYER_FOREGROUND);
  Tile *midgroundTile = world_map -> tile_at (inter_x, inter_y, LAYER_MIDGROUND);

  // Hoe
  if (inHand -> getID() == ITEM_HOE) {
    if (midgroundTile && !foregroundTile) {
      if (midgroundTile -> getID() == TILE_GRASS) {
        world_map -> replace_tile (midgroundTile, new Tile(TILE_PLOWED_SOIL, midgroundTile -> getX(), midgroundTile -> getY(), midgroundTile -> getZ()));
        SoundManager::play (SOUND_HOE);
      }
      else if (midgroundTile -> getID() == TILE_SOIL) {
        world_map -> replace_tile (midgroundTile, new Tile(TILE_PLOWED_SOIL, midgroundTile -> getX(), midgroundTile -> getY(), midgroundTile -> getZ()));
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
      world_map -> remove_tile (foregroundTile);
      place_item (new Item(ITEM_HAY, 0), midgroundTile -> getX(), midgroundTile -> getY());
      SoundManager::play (SOUND_SCYTHE);
    }
    else {
      map_messages -> push_message ("You can't cut there");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand -> getID() == ITEM_BERRY_SEED) {
    if (midgroundTile && midgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      world_map -> place_tile (new Tile (TILE_BERRY, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand -> getID() == ITEM_TOMATO_SEED) {
    if (midgroundTile && midgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      world_map -> place_tile (new Tile (TILE_TOMATO, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand -> getID() == ITEM_CARROT_SEED) {
    if (midgroundTile && midgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      world_map -> place_tile (new Tile (TILE_CARROT, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand -> getID() == ITEM_LAVENDER_SEED) {
    if (midgroundTile && midgroundTile -> getID() == TILE_PLOWED_SOIL && !foregroundTile) {
      world_map -> place_tile (new Tile (TILE_LAVENDER, inter_x, inter_y, 1));
    }
    else{
      map_messages -> push_message ("You must plant in ploughed soil");
      SoundManager::play (SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand -> getID() == ITEM_WATERING_CAN) {
    if (midgroundTile && midgroundTile -> getID() == TILE_WELL_PATH) {
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
      world_map -> replace_tile (foregroundTile, new Tile(TILE_STUMP, foregroundTile -> getX(), foregroundTile -> getY(), foregroundTile -> getZ()));
      place_item (new Item(ITEM_STICK, 0), foregroundTile -> getX(), foregroundTile -> getY());
      place_item (new Item(ITEM_STICK, 0), foregroundTile -> getX(), foregroundTile -> getY());
      place_item (new Item(ITEM_WOOD, 0), foregroundTile -> getX(), foregroundTile -> getY());
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
      world_map -> remove_tile (foregroundTile);
      SoundManager::play (SOUND_SHOVEL);
    }
    else if ( midgroundTile && midgroundTile -> getID() == TILE_GRASS && !foregroundTile) {
      world_map -> replace_tile (midgroundTile, new Tile(TILE_PATH, midgroundTile -> getX(), midgroundTile -> getY(), midgroundTile -> getZ()));
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
  // Update tile map
  world_map -> update(x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM, y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);

  // Regen map
  if (key[KEY_R]) {
    for (std::vector<MapItem*>::iterator i = map_items.begin(); i != map_items.end(); ++i) {
      delete *i;
    }
    map_items.clear();

    world_map -> clear_map();
    world_map -> generate_map();
  }

  if (ticks >= 10) {
    ticks = 0;

    // Run tickers for itens
    /*for (unsigned int i = 0; i < map_items.size(); i++) {
      // Current item
      Item *current = map_items.at(i) -> itemPtr;

      // Tile near item
      //Tile *foregroundTile = tile_at (map_items.at(i) -> x, map_items.at(i) -> y, FOREGROUND);
      Tile *backgroundTile = world_map -> tile_at (map_items.at(i) -> x, map_items.at(i) -> y, LAYER_BACKGROUND);

      // Chicken eggs
      if (current -> getID() == ITEM_CHICKEN) {
        if (backgroundTile && backgroundTile -> getID() == TILE_PATCHY_GRASS) {
          if (!random(0,32)) {
            int rand_1 = 16 * random (-1, 1);
            int rand_2 = 16 * random (-1, 1);
            if (!item_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2) &&
                !world_map -> solid_at (map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2)) {
              place_item (new Item (ITEM_EGG), map_items.at(i) -> x + rand_1, map_items.at(i) -> y + rand_2);
              SoundManager::play (SOUND_EGG);
            }
          }
        }
      }
    }*/

    // Foreground tiles
    //for (unsigned int i = 0; i < world_map -> map_tiles.size(); i++) {
      // Current tile
      //Tile *current = world_map -> map_tiles.at(i);
      //Tile *backgroundTile = world_map -> tile_at (current -> getX(), current -> getY(), LAYER_BACKGROUND);

      // Berries
      /*if (current -> getID() == TILE_BERRY) {
        // Grow a bit
        if (true)
          current -> changeMeta(1);
        // Done Growing
        if (current -> getMeta() >= MAX_TILE_META) {
          place_item (new Item(ITEM_BERRY), current -> getX(), current -> getY());
          world_map -> remove_tile (current);
          if (backgroundTile)
            world_map -> replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
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
          world_map -> remove_tile (current);
          if (backgroundTile)
            world_map -> replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
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
          world_map -> remove_tile (current);
          if (backgroundTile)
            world_map -> replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
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
          world_map -> remove_tile (current);
          if (backgroundTile)
            world_map -> replace_tile (backgroundTile, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()));
        }
      }*/
      // Plowed soil to grass
      /*else if (current -> getID() == TILE_PLOWED_SOIL) {
        if (!random (0, 1000))
          world_map -> replace_tile (current, new Tile(TILE_GRASS, backgroundTile -> getX(), backgroundTile -> getY(), backgroundTile -> getZ()), LAYER_BACKGROUND);
      }*/
    //}
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
  if ((player_x + (128 + 32) / VIEWPORT_ZOOM) - x > VIEWPORT_WIDTH / VIEWPORT_ZOOM && x + VIEWPORT_WIDTH / VIEWPORT_ZOOM < world_map -> getWidth() * 16)
    x += 4;
  else if ((player_x - 128 / VIEWPORT_ZOOM) - x < 0 && x > 0)
    x -= 4;

  // Scroll y
  if ((player_y + (96 + 32) / VIEWPORT_ZOOM) - y > VIEWPORT_HEIGHT / VIEWPORT_ZOOM && y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM < world_map -> getHeight() * 16)
    y += 4;
  else if ((player_y - 96 / VIEWPORT_ZOOM) - y < 0 && y > 0)
    y -= 4;
}
