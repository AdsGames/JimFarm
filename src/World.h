#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>
#include <asw/util/Timer.h>

#include <memory>

#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"

#include "TileMap.h"

// Viewport
constexpr int VIEWPORT_WIDTH = 240 * 2;
constexpr int VIEWPORT_HEIGHT = 160 * 2;
constexpr float VIEWPORT_MAX_ZOOM = 2.0f;
constexpr float VIEWPORT_MIN_ZOOM = 0.5f;

class World {
 public:
  World();

  // Drawing
  void draw();

  void loadImages();

  // Interact with
  void interact(int inter_x, int inter_y, std::shared_ptr<Item> inHand);

  // Map
  void update();
  void scroll(int scroll_x, int scroll_y);

  // Get x and y
  int getX() const;
  int getY() const;
  float getZoom() const;

  // Get map
  TileMap& getMap();

  // Get messenger
  Messenger& getMessenger();

 private:
  // Tile map
  TileMap tile_map;

  // Scroll position
  int x{0};
  int y{0};

  // Zoom
  float zoom{2.0f};

  // Buffer that holds whole map image
  asw::Texture map_buffer{nullptr};
  asw::Texture overlay_buffer{nullptr};

  // Ticker for world
  Timer ticker{};

  // Messager
  Messenger map_messages{Messenger(1, false, -4)};
};

#endif  // SRC_WORLD_H_
