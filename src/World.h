#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>
#include <asw/util/Timer.h>

#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"

#include "TileMap.h"

class World {
 public:
  World();

  // Constant
  int timer;

  // Viewport
  static constexpr int VIEWPORT_WIDTH = 240 * 2;
  static constexpr int VIEWPORT_HEIGHT = 160 * 2;
  static constexpr float VIEWPORT_MAX_ZOOM = 2.0f;
  static constexpr float VIEWPORT_MIN_ZOOM = 0.5f;

  float VIEWPORT_ZOOM;

  // Drawing
  void draw();

  void loadImages();

  // Interact with
  void interact(int inter_x, int inter_y, Item* inHand);

  // Map
  void update();
  void scroll(int player_x, int player_y);

  // Get x and y
  int getX() { return this->x; }
  int getY() { return this->y; }

  // Get messenger
  Messenger* getMessenger() { return this->map_messages; }

  // Tile map
  TileMap* world_map;

 private:
  // Scroll position
  int x, y;

  // Buffer that holds whole map image
  asw::Texture map_buffer;
  asw::Texture overlay_buffer;

  // Ticker for world
  Timer ticker;

  // Messager
  Messenger* map_messages;
};

#endif  // SRC_WORLD_H_
