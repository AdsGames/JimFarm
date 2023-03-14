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

/************
 * TILE MAP *
 ************/
World::World() {
  ticker.start();
  resetCamera();
}

void World::resetCamera() {
  Quad<int> bounds = {
      .x_1 = 0,
      .y_1 = 0,
      .x_2 = VIEWPORT_WIDTH,
      .y_2 = VIEWPORT_HEIGHT,
  };

  Quad<int> outer_bounds = {
      .x_1 = 0,
      .y_1 = 0,
      .x_2 = tile_map.getWidth() * TILE_SIZE,
      .y_2 = tile_map.getHeight() * TILE_SIZE,
  };

  camera = Camera(bounds, outer_bounds);
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw() {
  // Clear buffer
  SDL_SetRenderTarget(asw::display::renderer, map_buffer.get());

  // Drawable
  Graphics::Instance()->draw(camera);

  SDL_SetRenderTarget(asw::display::renderer, nullptr);

  SDL_SetTextureBlendMode(map_buffer.get(), SDL_BLENDMODE_BLEND);

  // Draw buffer
  asw::draw::stretchSpriteBlit(map_buffer, 0, 0, camera.getWidth(),
                               camera.getHeight(), 0, 0, VIEWPORT_WIDTH,
                               VIEWPORT_HEIGHT);

  // Draw temperature indicator
  const char temp =
      tile_map.getTemperatureAt(camera.getX() + camera.getWidth() / 2,
                                camera.getY() + camera.getHeight() / 2);
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
  map_messages.draw(5, 145);
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
  map_buffer = asw::assets::createTexture(
      static_cast<int>(VIEWPORT_WIDTH * VIEWPORT_MAX_ZOOM),
      static_cast<int>(VIEWPORT_HEIGHT * VIEWPORT_MAX_ZOOM));

  overlay_buffer = asw::assets::createTexture(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  tile_map.generateMap();

  resetCamera();
}

/*
 * MAP
 */
// Interact with
void World::interact(int inter_x, int inter_y, std::shared_ptr<Item> inHand) {
  auto tile_x = inter_x / TILE_SIZE;
  auto tile_y = inter_y / TILE_SIZE;

  std::shared_ptr<Tile> tile_f =
      tile_map.getTileAt(tile_x, tile_y, LAYER_FOREGROUND);
  std::shared_ptr<Tile> tile_m =
      tile_map.getTileAt(tile_x, tile_y, LAYER_MIDGROUND);
  std::shared_ptr<Tile> tile_b =
      tile_map.getTileAt(tile_x, tile_y, LAYER_BACKGROUND);

  // Hoe
  if (inHand->getId() == ITEM_HOE) {
    if (tile_m && !tile_f) {
      if (tile_m->getId() == TILE_GRASS) {
        auto plowed_tile = std::make_shared<Tile>(
            TILE_PLOWED_SOIL, tile_m->getX(), tile_m->getY(), tile_m->getZ());

        tile_map.replaceTile(tile_m, plowed_tile);

        SoundManager::play(SOUND_HOE);
      } else if (tile_m->getId() == TILE_SOIL) {
        auto plowed_tile = std::make_shared<Tile>(
            TILE_PLOWED_SOIL, tile_m->getX(), tile_m->getY(), tile_m->getZ());

        tile_map.replaceTile(tile_m, plowed_tile);

        SoundManager::play(SOUND_HOE);
      } else {
        SoundManager::play(SOUND_ERROR);
      }
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Scythe
  else if (inHand->getId() == ITEM_SCYTHE) {
    if (tile_f && tile_f->getId() == TILE_DENSE_GRASS) {
      tile_map.removeTile(tile_f);

      auto hay_item = std::make_shared<Item>(ITEM_HAY, 0);
      tile_map.placeItemAt(hay_item, tile_f->getTileX(), tile_f->getTileY());
      SoundManager::play(SOUND_SCYTHE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand->getId() == ITEM_BERRY_SEED) {
    if (tile_m && tile_m->getId() == TILE_PLOWED_SOIL && !tile_f) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_BERRY, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand->getId() == ITEM_TOMATO_SEED) {
    if (tile_m && tile_m->getId() == TILE_PLOWED_SOIL && !tile_f) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_TOMATO, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand->getId() == ITEM_CARROT_SEED) {
    if (tile_m && tile_m->getId() == TILE_PLOWED_SOIL && !tile_f) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_CARROT, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand->getId() == ITEM_LAVENDER_SEED) {
    if (tile_m && tile_m->getId() == TILE_PLOWED_SOIL && !tile_f) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_LAVENDER, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand->getId() == ITEM_WATERING_CAN) {
    if (tile_m && tile_m->getId() == TILE_WELL_PATH) {
      map_messages.pushMessage("Watering can filled");
      inHand->setMeta(8);
      SoundManager::play(SOUND_WATER_FILL);
    } else if (inHand->getMeta() > 0) {
      inHand->changeMeta(-1);
      map_messages.pushMessage("Watered");
      SoundManager::play(SOUND_WATER_POUR);
    } else {
      map_messages.pushMessage("Out of water");
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Axe
  else if (inHand->getId() == ITEM_AXE) {
    if (tile_f && tile_f->getId() == TILE_TREE) {
      tile_map.replaceTile(
          tile_f, std::make_shared<Tile>(TILE_STUMP, tile_f->getX(),
                                         tile_f->getY(), tile_f->getZ()));
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STICK, 0),
                           tile_f->getTileX(), tile_f->getTileY());
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STICK, 0),
                           tile_f->getTileX(), tile_f->getTileY());
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_WOOD, 0),
                           tile_f->getTileX(), tile_f->getTileY());
      SoundManager::play(SOUND_AXE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Shovel
  else if (inHand->getId() == ITEM_SHOVEL) {
    if (tile_f &&
        (tile_f->getId() == TILE_BUSH || tile_f->getId() == TILE_STUMP)) {
      tile_map.removeTile(tile_f);
      SoundManager::play(SOUND_SHOVEL);
    } else if (tile_m && tile_m->getId() == TILE_GRASS && !tile_f) {
      tile_map.removeTile(tile_m);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_GRASS, 0),
                           tile_m->getTileX(), tile_m->getTileY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (tile_m && tile_m->getId() == TILE_SAND && !tile_f) {
      tile_map.removeTile(tile_m);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_SAND, 0),
                           tile_m->getTileX(), tile_m->getTileY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (tile_f && (tile_f->getId() == TILE_STONE_WALL)) {
      tile_map.removeTile(tile_f);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STONE, 0),
                           tile_f->getTileX(), tile_f->getTileY());
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Wood Wall
  else if (inHand->getId() == ITEM_WOOD) {
    if (!tile_f && tile_m) {
      tile_map.placeTile(std::make_shared<Tile>(
          TILE_WOOD_WALL, tile_m->getX(), tile_m->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Fence
  else if (inHand->getId() == ITEM_STICK) {
    if (!tile_f && tile_m) {
      tile_map.placeTile(std::make_shared<Tile>(
          TILE_FENCE, tile_m->getX(), tile_m->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getId() == ITEM_GRASS) {
    if (tile_b && !tile_m) {
      tile_map.placeTile(std::make_shared<Tile>(
          TILE_GRASS, tile_b->getX(), tile_b->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getId() == ITEM_SAND) {
    if (tile_b && !tile_m) {
      tile_map.placeTile(std::make_shared<Tile>(
          TILE_SAND, tile_b->getX(), tile_b->getY(), LAYER_MIDGROUND));
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
    tile_map.clearMap();
    tile_map.generateMap();
  }

  // One game tick (20x second, 50ms)
  auto tickTime = ticker.getElapsedTime<std::chrono::milliseconds>();
  if (tickTime >= 50) {
    ticker.reset();

    // Update tile map
    tile_map.tick(camera);
  }

  // Zooming
  const auto zoom = camera.getZoom();

  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_PLUS] &&
      zoom < VIEWPORT_MAX_ZOOM) {
    camera.setZoom(zoom * 2.0f);
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_MINUS] &&
      zoom > VIEWPORT_MIN_ZOOM) {
    camera.setZoom(zoom * 0.5f);
  }
}

// Get map
TileMap& World::getMap() {
  return this->tile_map;
}

// Get messenger
Messenger& World::getMessenger() {
  return this->map_messages;
}

Camera& World::getCamera() {
  return camera;
}