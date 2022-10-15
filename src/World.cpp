#include "World.h"

#include <iostream>

#include "manager/InterfaceTypeManager.h"
#include "manager/ItemTypeManager.h"
#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

#include "manager/item_defs.h"
#include "manager/sound_defs.h"
#include "manager/tile_defs.h"

#include "manager/SoundManager.h"

#include "Graphics.h"

bool comparePtrToNode(Tile* a, Tile* b) {
  return (*a < *b);
}

/************
 * TILE MAP *
 ************/
World::World() {
  x = 0;
  y = 0;

  map_buffer = nullptr;

  map_messages = new Messenger(1, false, -4);

  VIEWPORT_ZOOM = 1.0f;

  world_map = new TileMap();

  ticker.start();
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw() {
  // Clear buffer
  SDL_SetRenderTarget(asw::display::renderer, map_buffer.get());

  // Drawable
  Graphics::Instance()->draw(x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM,
                             y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);

  SDL_SetRenderTarget(asw::display::renderer, nullptr);

  SDL_SetTextureBlendMode(map_buffer.get(), SDL_BLENDMODE_BLEND);

  // Draw buffer
  asw::draw::stretchSpriteBlit(map_buffer, 0, 0, VIEWPORT_WIDTH / VIEWPORT_ZOOM,
                               VIEWPORT_HEIGHT / VIEWPORT_ZOOM, 0, 0,
                               VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  // Draw temperature indicator
  const char temp =
      world_map->getTemperatureAt(x + (VIEWPORT_WIDTH / VIEWPORT_ZOOM) / 2,
                                  y + (VIEWPORT_HEIGHT / VIEWPORT_ZOOM) / 2);
  const char r_val = (temp > 0) ? (temp / 2) : 0;
  const char b_val = (temp < 0) ? (temp / 2 * -1) : 0;

  SDL_SetRenderTarget(asw::display::renderer, overlay_buffer.get());

  if (r_val > 0) {
    asw::draw::rectFill(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                        asw::util::makeColor(255, 0, 0, r_val));
  }
  if (b_val > 0) {
    asw::draw::rectFill(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                        asw::util::makeColor(0, 0, 255, b_val));
  }

  SDL_SetRenderTarget(asw::display::renderer, nullptr);

  SDL_SetTextureBlendMode(overlay_buffer.get(), SDL_BLENDMODE_BLEND);

  asw::draw::sprite(overlay_buffer, 0, 0);

  // Message system
  map_messages->draw(5, 145);
}

// Load images
void World::loadImages() {
  TileTypeManager::sprite_sheet_tiles =
      asw::assets::loadTexture("assets/images/tiles.png");
  ItemTypeManager::sprite_sheet_items =
      asw::assets::loadTexture("assets/images/items.png");

  std::cout << "Loading assets/data/tiles.json" << std::endl;
  if (TileTypeManager::loadTiles("assets/data/tiles.json")) {
    asw::util::abortOnError("Could not load assets/data/tiles.json");
  }

  std::cout << "Loading assets/data/items.json" << std::endl;
  if (ItemTypeManager::loadItems("assets/data/items.json")) {
    asw::util::abortOnError("Could not load assets/data/items.json");
  }

  std::cout << "Loading assets/data/interfaces.json" << std::endl;
  if (InterfaceTypeManager::loadInterfaces("assets/data/interfaces.json")) {
    asw::util::abortOnError("Could not load assets/data/interfaces.json");
  }

  std::cout << "Loading assets/data/sounds.json" << std::endl;
  if (SoundManager::load("assets/data/sounds.json")) {
    asw::util::abortOnError("Could not load assets/data/sounds.json");
  }

  // Create map buffer
  map_buffer = asw::assets::createTexture(VIEWPORT_WIDTH * VIEWPORT_MAX_ZOOM,
                                          VIEWPORT_HEIGHT * VIEWPORT_MAX_ZOOM);

  overlay_buffer = asw::assets::createTexture(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  world_map->generateMap();
}

/*
 * MAP
 */
// Interact with
void World::interact(int inter_x, int inter_y, Item* inHand) {
  Tile* foregroundTile =
      world_map->getTileAt(inter_x, inter_y, LAYER_FOREGROUND);
  Tile* midgroundTile = world_map->getTileAt(inter_x, inter_y, LAYER_MIDGROUND);
  Tile* backgroundTile =
      world_map->getTileAt(inter_x, inter_y, LAYER_BACKGROUND);

  // Hoe
  if (inHand->getID() == ITEM_HOE) {
    if (midgroundTile && !foregroundTile) {
      if (midgroundTile->getID() == TILE_GRASS) {
        world_map->replaceTile(
            midgroundTile,
            new Tile(TILE_PLOWED_SOIL, midgroundTile->getX(),
                     midgroundTile->getY(), midgroundTile->getZ()));
        SoundManager::play(SOUND_HOE);
      } else if (midgroundTile->getID() == TILE_SOIL) {
        world_map->replaceTile(
            midgroundTile,
            new Tile(TILE_PLOWED_SOIL, midgroundTile->getX(),
                     midgroundTile->getY(), midgroundTile->getZ()));
        SoundManager::play(SOUND_HOE);
      } else {
        SoundManager::play(SOUND_ERROR);
      }
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Scythe
  else if (inHand->getID() == ITEM_SCYTHE) {
    if (foregroundTile && foregroundTile->getID() == TILE_DENSE_GRASS) {
      world_map->removeTile(foregroundTile);
      world_map->placeItemAt(new Item(ITEM_HAY, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_SCYTHE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand->getID() == ITEM_BERRY_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_BERRY, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand->getID() == ITEM_TOMATO_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_TOMATO, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand->getID() == ITEM_CARROT_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_CARROT, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand->getID() == ITEM_LAVENDER_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_LAVENDER, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand->getID() == ITEM_WATERING_CAN) {
    if (midgroundTile && midgroundTile->getID() == TILE_WELL_PATH) {
      map_messages->pushMessage("Watering can filled");
      inHand->setMeta(8);
      SoundManager::play(SOUND_WATER_FILL);
    } else if (inHand->getMeta() > 0) {
      inHand->changeMeta(-1);
      map_messages->pushMessage("Watered");
      SoundManager::play(SOUND_WATER_POUR);
    } else {
      map_messages->pushMessage("Out of water");
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Axe
  else if (inHand->getID() == ITEM_AXE) {
    if (foregroundTile && foregroundTile->getID() == TILE_TREE) {
      world_map->replaceTile(
          foregroundTile,
          new Tile(TILE_STUMP, foregroundTile->getX(), foregroundTile->getY(),
                   foregroundTile->getZ()));
      world_map->placeItemAt(new Item(ITEM_STICK, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      world_map->placeItemAt(new Item(ITEM_STICK, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      world_map->placeItemAt(new Item(ITEM_WOOD, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_AXE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Shovel
  else if (inHand->getID() == ITEM_SHOVEL) {
    if (foregroundTile && (foregroundTile->getID() == TILE_BUSH ||
                           foregroundTile->getID() == TILE_STUMP)) {
      world_map->removeTile(foregroundTile);
      SoundManager::play(SOUND_SHOVEL);
    } else if (midgroundTile && midgroundTile->getID() == TILE_GRASS &&
               !foregroundTile) {
      world_map->removeTile(midgroundTile);
      world_map->placeItemAt(new Item(ITEM_GRASS, 0), midgroundTile->getX(),
                             midgroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (midgroundTile && midgroundTile->getID() == TILE_SAND &&
               !foregroundTile) {
      world_map->removeTile(midgroundTile);
      world_map->placeItemAt(new Item(ITEM_SAND, 0), midgroundTile->getX(),
                             midgroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (foregroundTile && (foregroundTile->getID() == TILE_STONE_WALL)) {
      world_map->removeTile(foregroundTile);
      world_map->placeItemAt(new Item(ITEM_STONE, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Wood Wall
  else if (inHand->getID() == ITEM_WOOD) {
    if (!foregroundTile && midgroundTile) {
      world_map->placeTile(new Tile(TILE_WOOD_WALL, midgroundTile->getX(),
                                    midgroundTile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Fence
  else if (inHand->getID() == ITEM_STICK) {
    if (!foregroundTile && midgroundTile) {
      world_map->placeTile(new Tile(TILE_FENCE, midgroundTile->getX(),
                                    midgroundTile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getID() == ITEM_GRASS) {
    if (backgroundTile && !midgroundTile) {
      world_map->placeTile(new Tile(TILE_GRASS, backgroundTile->getX(),
                                    backgroundTile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getID() == ITEM_SAND) {
    if (backgroundTile && !midgroundTile) {
      world_map->placeTile(new Tile(TILE_SAND, backgroundTile->getX(),
                                    backgroundTile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
}

// Update tile map
void World::update() {
  // Regen map
  if (asw::input::keyboard.down[SDL_SCANCODE_R]) {
    world_map->clearMap();
    world_map->generateMap();
  }

  // One game tick (20x second, 50ms)
  auto tickTime = ticker.getElapsedTime<std::chrono::milliseconds>();
  if (tickTime >= 50) {
    ticker.reset();

    // Update tile map
    world_map->tick(x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM,
                    y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);
  }

  // Zooming
  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_PLUS] &&
      VIEWPORT_ZOOM < VIEWPORT_MAX_ZOOM) {
    VIEWPORT_ZOOM *= 2.0f;
  }
  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_MINUS] &&
      VIEWPORT_ZOOM > VIEWPORT_MIN_ZOOM) {
    VIEWPORT_ZOOM *= 0.5f;
  }
}

// Scroll
void World::scroll(int player_x, int player_y) {
  x = player_x + (TILE_WIDTH - VIEWPORT_WIDTH / VIEWPORT_ZOOM) / 2;
  y = player_y + (TILE_HEIGHT - VIEWPORT_HEIGHT / VIEWPORT_ZOOM) / 2;

  if (x < 0) {
    x = 0;
  } else if (x > world_map->getWidth() * TILE_WIDTH -
                     VIEWPORT_WIDTH / VIEWPORT_ZOOM) {
    x = world_map->getWidth() * TILE_WIDTH - VIEWPORT_WIDTH / VIEWPORT_ZOOM;
  }

  if (y < 0) {
    y = 0;
  } else if (y > world_map->getHeight() * TILE_HEIGHT -
                     VIEWPORT_HEIGHT / VIEWPORT_ZOOM) {
    y = world_map->getHeight() * TILE_HEIGHT - VIEWPORT_HEIGHT / VIEWPORT_ZOOM;
  }
}
