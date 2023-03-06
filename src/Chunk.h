#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "MapItem.h"
#include "Tile.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 16;
const int CHUNK_LAYERS = 5;

const int LAYER_BACKGROUND = 0;
const int LAYER_MIDGROUND = 1;
const int LAYER_CHARACTER = 2;
const int LAYER_ITEMS = 3;
const int LAYER_FOREGROUND = 4;

class Chunk {
 public:
  Chunk(int x, int y);

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
  std::shared_ptr<Tile> tiles[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LAYERS];

  // Biome
  char temperature[CHUNK_WIDTH][CHUNK_HEIGHT];
  char rainfall[CHUNK_WIDTH][CHUNK_HEIGHT];
  char height[CHUNK_WIDTH][CHUNK_HEIGHT];

  // Items
  std::vector<std::shared_ptr<MapItem>> items;

  // Generate
  void generate();

  int x = 0;
  int y = 0;

  bool is_drawing = false;
};

#endif  // SRC_CHUNK_H_
