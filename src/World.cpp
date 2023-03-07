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

bool comparePtrToNode(std::shared_ptr<Tile> a, std::shared_ptr<Tile> b) {
  return (*a < *b);
}

/************
 * TILE MAP *
 ************/
World::World() {
  ticker.start();
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw() {
  // Clear buffer
  SDL_SetRenderTarget(asw::display::renderer, map_buffer.get());

  const auto v_width = static_cast<int>(VIEWPORT_WIDTH / zoom);
  const auto v_height = static_cast<int>(VIEWPORT_HEIGHT / zoom);

  // Drawable
  Graphics::Instance()->draw(x, y, x + v_width, y + v_height);

  SDL_SetRenderTarget(asw::display::renderer, nullptr);

  SDL_SetTextureBlendMode(map_buffer.get(), SDL_BLENDMODE_BLEND);

  // Draw buffer
  asw::draw::stretchSpriteBlit(map_buffer, 0, 0, v_width, v_height, 0, 0,
                               VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  // Draw temperature indicator
  const char temp =
      tile_map.getTemperatureAt(x + v_width / 2, y + v_height / 2);
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
}

/*
 * MAP
 */
// Interact with
void World::interact(int inter_x, int inter_y, std::shared_ptr<Item> inHand) {
  std::shared_ptr<Tile> foregroundTile =
      tile_map.getTileAt(inter_x, inter_y, LAYER_FOREGROUND);
  std::shared_ptr<Tile> midground_tile =
      tile_map.getTileAt(inter_x, inter_y, LAYER_MIDGROUND);
  std::shared_ptr<Tile> background_tile =
      tile_map.getTileAt(inter_x, inter_y, LAYER_BACKGROUND);

  // Hoe
  if (inHand->getId() == ITEM_HOE) {
    if (midground_tile && !foregroundTile) {
      if (midground_tile->getId() == TILE_GRASS) {
        tile_map.replaceTile(
            midground_tile,
            std::make_shared<Tile>(TILE_PLOWED_SOIL, midground_tile->getX(),
                                   midground_tile->getY(),
                                   midground_tile->getZ()));
        SoundManager::play(SOUND_HOE);
      } else if (midground_tile->getId() == TILE_SOIL) {
        tile_map.replaceTile(
            midground_tile,
            std::make_shared<Tile>(TILE_PLOWED_SOIL, midground_tile->getX(),
                                   midground_tile->getY(),
                                   midground_tile->getZ()));
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
    if (foregroundTile && foregroundTile->getId() == TILE_DENSE_GRASS) {
      tile_map.removeTile(foregroundTile);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_HAY, 0),
                           foregroundTile->getX(), foregroundTile->getY());
      SoundManager::play(SOUND_SCYTHE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand->getId() == ITEM_BERRY_SEED) {
    if (midground_tile && midground_tile->getId() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_BERRY, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand->getId() == ITEM_TOMATO_SEED) {
    if (midground_tile && midground_tile->getId() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_TOMATO, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand->getId() == ITEM_CARROT_SEED) {
    if (midground_tile && midground_tile->getId() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_CARROT, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand->getId() == ITEM_LAVENDER_SEED) {
    if (midground_tile && midground_tile->getId() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      tile_map.placeTile(std::make_shared<Tile>(TILE_LAVENDER, inter_x, inter_y,
                                                LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand->getId() == ITEM_WATERING_CAN) {
    if (midground_tile && midground_tile->getId() == TILE_WELL_PATH) {
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
    if (foregroundTile && foregroundTile->getId() == TILE_TREE) {
      tile_map.replaceTile(foregroundTile,
                           std::make_shared<Tile>(
                               TILE_STUMP, foregroundTile->getX(),
                               foregroundTile->getY(), foregroundTile->getZ()));
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STICK, 0),
                           foregroundTile->getX(), foregroundTile->getY());
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STICK, 0),
                           foregroundTile->getX(), foregroundTile->getY());
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_WOOD, 0),
                           foregroundTile->getX(), foregroundTile->getY());
      SoundManager::play(SOUND_AXE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Shovel
  else if (inHand->getId() == ITEM_SHOVEL) {
    if (foregroundTile && (foregroundTile->getId() == TILE_BUSH ||
                           foregroundTile->getId() == TILE_STUMP)) {
      tile_map.removeTile(foregroundTile);
      SoundManager::play(SOUND_SHOVEL);
    } else if (midground_tile && midground_tile->getId() == TILE_GRASS &&
               !foregroundTile) {
      tile_map.removeTile(midground_tile);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_GRASS, 0),
                           midground_tile->getX(), midground_tile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (midground_tile && midground_tile->getId() == TILE_SAND &&
               !foregroundTile) {
      tile_map.removeTile(midground_tile);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_SAND, 0),
                           midground_tile->getX(), midground_tile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (foregroundTile && (foregroundTile->getId() == TILE_STONE_WALL)) {
      tile_map.removeTile(foregroundTile);
      tile_map.placeItemAt(std::make_shared<Item>(ITEM_STONE, 0),
                           foregroundTile->getX(), foregroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Wood Wall
  else if (inHand->getId() == ITEM_WOOD) {
    if (!foregroundTile && midground_tile) {
      tile_map.placeTile(
          std::make_shared<Tile>(TILE_WOOD_WALL, midground_tile->getX(),
                                 midground_tile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Fence
  else if (inHand->getId() == ITEM_STICK) {
    if (!foregroundTile && midground_tile) {
      tile_map.placeTile(
          std::make_shared<Tile>(TILE_FENCE, midground_tile->getX(),
                                 midground_tile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getId() == ITEM_GRASS) {
    if (background_tile && !midground_tile) {
      tile_map.placeTile(
          std::make_shared<Tile>(TILE_GRASS, background_tile->getX(),
                                 background_tile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getId() == ITEM_SAND) {
    if (background_tile && !midground_tile) {
      tile_map.placeTile(
          std::make_shared<Tile>(TILE_SAND, background_tile->getX(),
                                 background_tile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
}

// Update tile map
void World::update() {
  auto v_width = static_cast<int>(VIEWPORT_WIDTH / zoom);
  auto v_height = static_cast<int>(VIEWPORT_HEIGHT / zoom);

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
    tile_map.tick(x, y, x + v_width, y + v_height);
  }

  // Zooming
  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_PLUS] &&
      zoom < VIEWPORT_MAX_ZOOM) {
    zoom *= 2.0f;
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_KP_MINUS] &&
      zoom > VIEWPORT_MIN_ZOOM) {
    zoom *= 0.5f;
  }
}

// Scroll
void World::scroll(int scroll_x, int scroll_y) {
  auto v_width = static_cast<int>(VIEWPORT_WIDTH / zoom);
  auto v_height = static_cast<int>(VIEWPORT_HEIGHT / zoom);

  x = scroll_x + (TILE_WIDTH - v_width) / 2;
  y = scroll_y + (TILE_HEIGHT - v_height) / 2;

  if (x < 0) {
    x = 0;
  } else if (x > tile_map.getWidth() * TILE_WIDTH - v_width) {
    x = tile_map.getWidth() * TILE_WIDTH - v_width;
  }

  if (y < 0) {
    y = 0;
  } else if (y > tile_map.getHeight() * TILE_HEIGHT - v_height) {
    y = tile_map.getHeight() * TILE_HEIGHT - v_height;
  }
}

// Get x and y
int World::getX() const {
  return this->x;
}

int World::getY() const {
  return this->y;
}

float World::getZoom() const {
  return this->zoom;
}

// Get map
TileMap& World::getMap() {
  return this->tile_map;
}

// Get messenger
Messenger& World::getMessenger() {
  return this->map_messages;
}