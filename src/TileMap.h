#ifndef TILEMAP_H
#define TILEMAP_H

#define M_PI 3.14159265

#include <string>
#include <vector>

#include "Chunk.h"
#include "Item.h"
#include "Tile.h"

class TileMap {
 public:
  TileMap();
  virtual ~TileMap();

  // Size
  int getWidth();
  int getHeight();

  // Chunk management
  Chunk* getChunkAt(int x, int y);

  std::string getBiomeAt(int x, int y);
  char getTemperatureAt(int x, int y);

  // Tiles
  Tile* getTileAt(int x, int y, int layer);
  void placeTile(Tile* newTile);
  void replaceTile(Tile* oldTile, Tile* newTile);
  void removeTile(Tile* newTile);
  bool isSolidAt(int x, int y);

  // Items
  MapItem* getItemAt(int x, int y);
  void placeItemAt(Item* item, int x, int y);
  void removeItem(MapItem* item);

  // Update
  void tick(int x_1, int y_1, int x_2, int y_2);

  // Loading
  void loadMap(std::string fileName);
  void generateMap();
  void clearMap();

  // Bitmasks
  void updateBitMask(Tile* newTile);
  void updateBitmaskSurround(Tile* newTile);

  // Chunks
  std::vector<std::vector<Chunk*>> chunks;

 private:
  // Size
  int width;
  int height;
};

#endif  // TILEMAP_H
