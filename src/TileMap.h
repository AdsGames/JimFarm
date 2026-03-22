#ifndef SRC_TILEMAP_H_
#define SRC_TILEMAP_H_

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "Chunk.h"
#include "Item.h"
#include "Tile.h"

class World;

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

  /**
   * @brief Swap a tile for another type in the same spot
   *
   * @param tile Tile to replace
   * @param id New tile id, empty removes the tile
   * @param meta Meta of the new tile
   */
  void replaceTile(const std::shared_ptr<Tile>& tile,
                   const std::string& id,
                   unsigned char meta = 0);

  /**
   * @brief Place a building wider than one tile. The anchor is the bottom
   * left tile, the rest of the bottom row is filled with solid parts that
   * point back to it.
   *
   * @param id Tile id
   * @param pos Anchor position in tile coordinates
   */
  void placeStructure(const std::string& id, const asw::Vec2i& pos);

  /**
   * @brief Resolve a structure part to its anchor tile
   *
   * @param tile Any tile
   * @return The anchor if tile is a structure part, else tile
   */
  std::shared_ptr<Tile> resolveStructure(const std::shared_ptr<Tile>& tile);

  // Is pos inside the map
  bool inBounds(const asw::Vec2i& pos) const;

  // Items
  std::shared_ptr<MapItem> getItemAt(const asw::Vec2i& pos);
  void placeItemAt(std::shared_ptr<Item> item, const asw::Vec2i& pos);
  void removeItem(std::shared_ptr<MapItem> item);

  // Update chunks near the camera, runs ticking behaviours
  void tick(const Camera& camera, World& world);

  // Run day end behaviours on every tile
  void dayEnd(World& world);

  // Swap every tile of one type for another (e.g. rain waters soil)
  void replaceAll(const std::string& from, const std::string& to);

  // Remove items lying in an area (tile coordinates, inclusive)
  void clearItems(const asw::Vec2i& from, const asw::Vec2i& to);

  // Loading
  void generateMap(const asw::Vec2<unsigned int>& size);
  void clearMap();

  // Saving
  nlohmann::json toJson() const;
  void fromJson(const nlohmann::json& data);

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

  // All tiles with behaviours, collected so behaviours may edit the map
  std::vector<std::shared_ptr<Tile>> collectTiles(bool visible_only,
                                                  bool ticking_only) const;

  const static std::array<std::pair<int, int>, 4> BITMASK_DIRECTIONS;
};

#endif  // SRC_TILEMAP_H_
