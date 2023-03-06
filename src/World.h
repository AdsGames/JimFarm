#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>
#include <asw/util/Timer.h>

#include <memory>

#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"

#include "TileMap.h"

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
  void scroll(int player_x, int player_y);

  // Get x and y
  int getX() const { return this->x; }
  int getY() const { return this->y; }

  // Get messenger
  const Messenger& getMessenger() const { return this->map_messages; }

  // Tile map
  std::shared_ptr<TileMap> world_map = nullptr;

  // Viewport
  static constexpr int VIEWPORT_WIDTH = 240 * 2;
  static constexpr int VIEWPORT_HEIGHT = 160 * 2;
  static constexpr float VIEWPORT_MAX_ZOOM = 2.0f;
  static constexpr float VIEWPORT_MIN_ZOOM = 0.5f;

  float VIEWPORT_ZOOM = 2.0f;

 private:
  // Scroll position
  int x = 0;
  int y = 0;

  // Buffer that holds whole map image
  asw::Texture map_buffer = nullptr;
  asw::Texture overlay_buffer = nullptr;

  // Ticker for world
  Timer ticker{};

  // Messager
  Messenger map_messages = Messenger(1, false, -4);
};

#endif  // SRC_WORLD_H_
