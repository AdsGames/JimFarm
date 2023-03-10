#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "MapItem.h"
#include "Tile.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 16;
constexpr int CHUNK_LAYERS = 3;
constexpr int CHUNK_LAYER_SIZE = CHUNK_WIDTH * CHUNK_HEIGHT;
constexpr int CHUNK_SIZE = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LAYERS;
constexpr int CHUNK_WIDTH_PX = CHUNK_WIDTH * TILE_WIDTH;
constexpr int CHUNK_HEIGHT_PX = CHUNK_HEIGHT * TILE_HEIGHT;

constexpr int LAYER_BACKGROUND = 0;
constexpr int LAYER_MIDGROUND = 1;
constexpr int LAYER_FOREGROUND = 2;

class Chunk {
 public:
  Chunk(int x, int y);
  ~Chunk();

  int getX() const;
  int getY() const;

  std::shared_ptr<Tile> getTileAt(int x, int y, int z) const;
  void setTileAt(int x, int y, int z, std::shared_ptr<Tile> tile);

  std::shared_ptr<MapItem> getItemAt(int x, int y) const;
  void placeItemAt(std::shared_ptr<Item> item, int x, int y);
  void removeItem(std::shared_ptr<MapItem> item);

  std::string getBiomeAt(int x, int y) const;
  char getTemperatureAt(int x, int y) const;

  void setDrawEnabled(bool enabled);

  bool getInRange(int x_1, int y_1, int x_2, int y_2);

  void tick();

  static int seed;

 private:
  // Tiles
  std::array<std::shared_ptr<Tile>, CHUNK_SIZE> tiles;

  // Biome
  std::array<char, CHUNK_LAYER_SIZE> temperature;
  std::array<char, CHUNK_LAYER_SIZE> rainfall;
  std::array<char, CHUNK_LAYER_SIZE> height;

  // Items
  std::vector<std::shared_ptr<MapItem>> items;

  // Generate
  void generate();

  int x = 0;
  int y = 0;

  bool is_drawing = false;
};

#endif  // SRC_CHUNK_H_
