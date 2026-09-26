#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>

#include <memory>

#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"
#include "TileMap.h"
#include "ui/Hud.h"
#include "utility/Camera.h"

// Viewport
constexpr int VIEWPORT_WIDTH = 240 * 4;
constexpr int VIEWPORT_HEIGHT = 160 * 4;
constexpr float VIEWPORT_MAX_ZOOM = 4.0f;
constexpr float VIEWPORT_MIN_ZOOM = 0.25f;

class World {
 public:
  World();

  // Drawing
  void draw();

  void loadImages();

  // Interact with
  void interact(const asw::Vec2i& inter_pos, std::shared_ptr<Item> in_hand);

  // Map
  void update(float dt);

  // Get map
  TileMap& getMap();

  // Get messenger
  Messenger& getMessenger();

  void resetCamera();

  Camera& getCamera();

 private:
  // Tile map
  TileMap tile_map;

  // Buffer that holds whole map image
  asw::Texture map_buffer{nullptr};
  asw::Texture overlay_buffer{nullptr};

  // Ticker for world
  float ticker{};

  // Camera config
  Camera camera{};

  // Messager
  Messenger map_messages{Messenger(1, false, -4)};

  // Hud
  Hud hud{};
};

#endif  // SRC_WORLD_H_
