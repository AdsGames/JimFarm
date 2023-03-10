#ifndef SRC_TILEMAP_H_
#define SRC_TILEMAP_H_

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Chunk.h"
#include "Item.h"
#include "Tile.h"

class TileMap {
 public:
  // Size
  int getWidth() const;
  int getHeight() const;

  /**
   * @brief Get the Biome at tile position x, y
   *
   * @param x Tile x position in tile coordinates
   * @param y Tile y position in tile coordinates
   * @return std::string Biome name
   */
  std::string getBiomeAt(int x, int y);

  /**
   * @brief Get the Temperature at tile position x, y
   *
   * @param x Tile x position in tile coordinates
   * @param y Tile y position in tile coordinates
   * @return char Temperature
   */
  char getTemperatureAt(int x, int y);

  /**
   * @brief Get the Tile position and layer
   *
   * @param x Tile x position in tile coordinates
   * @param y Tile y position in tile coordinates
   * @param layer Tile layer (z)
   * @return std::shared_ptr<Tile> Tile, if found
   */
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

 private:
  /**
   * @brief Get the Chunk at position x, y
   *
   * @param x X in tile coordinates
   * @param y Y in tile coordinates
   * @return std::shared_ptr<Chunk> Chunk, if found
   */
  std::shared_ptr<Chunk> getChunkAt(int x, int y);

  // Bitmasks
  void updateBitMask(std::shared_ptr<Tile> tile);
  void updateBitmaskSurround(int x, int y, int z);

  // Size
  int width{1};
  int height{1};

  // Chunks
  std::vector<std::vector<std::shared_ptr<Chunk>>> chunks;

  const static std::array<std::pair<int, int>, 4> BITMASK_DIRECTIONS;
};

#endif  // SRC_TILEMAP_H_
