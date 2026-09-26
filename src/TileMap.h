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
   * @param pos Position in tile coordinates
   * @return std::string Biome name
   */
  std::string getBiomeAt(const asw::Vec2i& pos);

  /**
   * @brief Get the Temperature at tile position x, y
   *
   * @param pos Position in tile coordinates
   * @return char Temperature
   */
  char getTemperatureAt(const asw::Vec2i& pos);

  /**
   * @brief Get the Tile position and layer
   *
   * @param pos Position in tile coordinates
   * @param layer Tile layer (z)
   * @return std::shared_ptr<Tile> Tile, if found
   */
  std::shared_ptr<Tile> getTileAt(const asw::Vec2i& pos, int layer);

  void placeTile(std::shared_ptr<Tile> tile);
  void removeTile(std::shared_ptr<Tile> tile);
  bool isSolidAt(const asw::Vec2i& pos);

  // Items
  std::shared_ptr<MapItem> getItemAt(const asw::Vec2i& pos);
  void placeItemAt(std::shared_ptr<Item> item, const asw::Vec2i& pos);
  void removeItem(std::shared_ptr<MapItem> item);

  // Update
  void tick(const Camera& camera) const;

  // Loading
  void generateMap(const asw::Vec2<unsigned int>& size);
  void clearMap();

 private:
  /**
   * @brief Get the Chunk at position x, y
   *
   * @param pos Chunk position in chunk coordinates
   * @return std::shared_ptr<Chunk> Chunk, if found
   */
  std::shared_ptr<Chunk> getChunkAt(const asw::Vec2i& pos);

  // Bitmasks
  void updateBitMask(std::shared_ptr<Tile> tile);
  void updateBitmaskSurround(const asw::Vec2i& pos, int z);

  // Size
  asw::Vec2<unsigned int> size{0, 0};

  // Chunks
  std::vector<std::vector<std::shared_ptr<Chunk>>> chunks;

  const static std::array<std::pair<int, int>, 4> BITMASK_DIRECTIONS;
};

#endif  // SRC_TILEMAP_H_
