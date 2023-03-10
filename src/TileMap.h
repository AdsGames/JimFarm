#ifndef SRC_TILEMAP_H_
#define SRC_TILEMAP_H_

#include <memory>
#include <string>
#include <vector>

#include "Chunk.h"
#include "Item.h"
#include "Tile.h"

class TileMap {
 public:
  // Size
  int getWidth() const;
  int getHeight() const;

  // Chunk management
  std::shared_ptr<Chunk> getChunkAt(int x, int y);

  std::string getBiomeAt(int x, int y);
  char getTemperatureAt(int x, int y);

  // Tiles
  std::shared_ptr<Tile> getTileAt(int x, int y, int layer);
  void placeTile(std::shared_ptr<Tile> tile);
  void replaceTile(std::shared_ptr<Tile> tile_old,
                   std::shared_ptr<Tile> tile_new);
  void removeTile(std::shared_ptr<Tile> tile);
  bool isSolidAt(int x, int y);

  // Items
  std::shared_ptr<MapItem> getItemAt(int x, int y);
  void placeItemAt(std::shared_ptr<Item> item, int x, int y);
  void removeItem(std::shared_ptr<MapItem> item);

  // Update
  void tick(int x_1, int y_1, int x_2, int y_2) const;

  // Loading
  void generateMap();
  void clearMap();

  // Bitmasks
  void updateBitMask(std::shared_ptr<Tile> tile);
  void updateBitmaskSurround(int x, int y, int z);

 private:
  // Size
  int width{1};
  int height{1};

  // Chunks
  std::vector<std::vector<std::shared_ptr<Chunk>>> chunks;
};

#endif  // SRC_TILEMAP_H_
