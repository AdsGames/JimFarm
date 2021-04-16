#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "MapItem.h"
#include "Tile.h"

#include <string>
#include <vector>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LAYERS 5

#define LAYER_BACKGROUND 0
#define LAYER_MIDGROUND 1
#define LAYER_CHARACTER 2
#define LAYER_ITEMS 3
#define LAYER_FOREGROUND 4

class Chunk {
 public:
  Chunk(int x, int y);
  virtual ~Chunk();

  int getX();
  int getY();

  Tile* getTileAt(int x, int y, int z);
  void setTileAt(int x, int y, int z, Tile* tile);

  MapItem* getItemAt(int x, int y);
  void placeItemAt(Item* item, int x, int y);
  void removeItem(MapItem* item);

  std::string getBiomeAt(int x, int y);
  char getTemperatureAt(int x, int y);

  void setDrawEnabled(bool enabled);

  bool getInRange(int x_1, int y_1, int x_2, int y_2);

  void tick();

  static int seed;

 private:
  // Tiles
  Tile* tiles[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LAYERS];

  // Biome
  char temperature[CHUNK_WIDTH][CHUNK_HEIGHT];
  char rainfall[CHUNK_WIDTH][CHUNK_HEIGHT];
  char height[CHUNK_WIDTH][CHUNK_HEIGHT];

  // Items
  std::vector<MapItem*> items;

  // Near tiles
  Chunk* neighbours[8];

  // Generate
  void generate();

  int x;
  int y;

  bool is_drawing;
};

#endif  // SRC_CHUNK_H_
