#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "MapItem.h"
#include "Tile.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_LAYERS = 3;

constexpr int LAYER_BACKGROUND = 0;
constexpr int LAYER_MIDGROUND = 1;
constexpr int LAYER_FOREGROUND = 2;

class Chunk {
 public:
  /**
   * @brief Construct a new Chunk object
   *
   * @param index_x - Chunk x index in chunk coordinates
   * @param index_y - Chunk y index in chunk coordinates
   */
  Chunk(int index_x, int index_y);
  ~Chunk();

  int getXIndex() const;
  int getYIndex() const;

  /**
   * @brief Get the Tile at position x, y, z
   *
   * @param x X position in local chunk coordinates
   * @param y Y position in local chunk coordinates
   * @param z Z position in local chunk coordinates
   * @return std::shared_ptr<Tile> Tile, if found
   */
  std::shared_ptr<Tile> getTileAt(int x, int y, int z) const;

  /**
   * @brief Set the Tile at position x, y, z
   *
   * @param x X position in local chunk coordinates
   * @param y Y position in local chunk coordinates
   * @param z Z position in local chunk coordinates
   * @param tile Tile to set
   */
  void setTileAt(int x, int y, int z, std::shared_ptr<Tile> tile);

  /**
   * @brief Get the Item at position x, y
   *
   * @param x X position in tile coordinates
   * @param y Y position in tile coordinates
   * @return std::shared_ptr<MapItem> Item, if found
   */
  std::shared_ptr<MapItem> getItemAt(int x, int y) const;

  /**
   * @brief Place an item at position x, y
   *
   * @param item Ptr to item
   * @param x X position in tile coordinates
   * @param y Y position in tile coordinates
   */
  void placeItemAt(std::shared_ptr<Item> item, int x, int y);

  /**
   * @brief Remove an item from the chunk
   *
   * @param item Ptr to item
   */
  void removeItem(std::shared_ptr<MapItem> item);

  /**
   * @brief Get the Biome at position x, y
   *
   * @param x X position in local chunk coordinates
   * @param y Y position in local chunk coordinates
   * @return std::string Biome name
   */
  std::string getBiomeAt(int x, int y) const;

  /**
   * @brief Get the Temperature at position x, y
   *
   * @param x X position in local chunk coordinates
   * @param y Y position in local chunk coordinates
   * @return char Temperature
   */
  char getTemperatureAt(int x, int y) const;

  /**
   * @brief Set drawing enabled for chunk
   *
   * @param enabled Enabled flag
   */
  void setDrawEnabled(bool enabled);

  /**
   * @brief Update chunk
   *
   */
  void tick();

  /**
   * @brief Seed for random number generation
   *
   */
  static int seed;

 private:
  /**
   * @brief Convert x, y, z to index
   *
   * @param x X position in local chunk coordinates
   * @param y Y position in local chunk coordinates
   * @param z Z position in local chunk coordinates
   * @return int Index in tiles array
   */
  size_t getTileIndex(unsigned int x, unsigned int y, unsigned int z) const;

  // Tiles
  std::array<std::shared_ptr<Tile>, CHUNK_SIZE * CHUNK_SIZE * CHUNK_LAYERS>
      tiles{nullptr};

  // Biome
  std::array<char, CHUNK_SIZE * CHUNK_SIZE> temperature{0};
  std::array<char, CHUNK_SIZE * CHUNK_SIZE> rainfall{0};
  std::array<char, CHUNK_SIZE * CHUNK_SIZE> height{0};

  // Items
  std::vector<std::shared_ptr<MapItem>> items{};

  /**
   * @brief Generate chunk
   *
   */
  void generate();

  /**
   * @brief Generate chunk biome helper
   *
   */
  void generateBiome();

  int index_x = 0;
  int index_y = 0;

  bool is_drawing = false;
};

#endif  // SRC_CHUNK_H_
