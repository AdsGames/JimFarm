#include "TileMap.h"

#include <math.h>
#include <time.h>
#include <exception>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Graphics.h"
#include "Item.h"

#include "manager/tile_defs.h"
#include "utility/Tools.h"

const std::array<std::pair<int, int>, 4> TileMap::BITMASK_DIRECTIONS = {
    {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

// Size
int TileMap::getWidth() const {
  return width * CHUNK_SIZE;
}

int TileMap::getHeight() const {
  return height * CHUNK_SIZE;
}

// Chunk lookup
std::shared_ptr<Chunk> TileMap::getChunkAt(int x, int y) {
  auto offset_x = x / CHUNK_SIZE;
  auto offset_y = y / CHUNK_SIZE;

  if (offset_y < 0 || offset_y >= (signed)chunks.size()) {
    return nullptr;
  }

  if (offset_x < 0 || offset_x >= (signed)chunks[offset_y].size()) {
    return nullptr;
  }

  return chunks.at(offset_y).at(offset_x);
}

std::string TileMap::getBiomeAt(int x, int y) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return "none";
  }
  return chunk->getBiomeAt(x % CHUNK_SIZE, y % CHUNK_SIZE);
}

char TileMap::getTemperatureAt(int x, int y) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return 0;
  }
  return chunk->getTemperatureAt(x % CHUNK_SIZE, y % CHUNK_SIZE);
}

// Get tile at position
std::shared_ptr<Tile> TileMap::getTileAt(int x, int y, int layer) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return nullptr;
  }

  return chunk->getTileAt(x % CHUNK_SIZE, y % CHUNK_SIZE, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw new std::runtime_error("Can not place tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTileX(), tile->getTileY());

  if (!chunk) {
    throw new std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->setTileAt(tile->getTileX() % CHUNK_SIZE, tile->getTileY() % CHUNK_SIZE,
                   tile->getZ(), tile);

  updateBitmaskSurround(tile->getTileX() % CHUNK_SIZE,
                        tile->getTileY() % CHUNK_SIZE, tile->getZ());
}

// Remove tile from map
void TileMap::removeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw new std::runtime_error("Can not remove tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTileX(), tile->getTileY());

  if (!chunk) {
    throw new std::runtime_error("Can not place tile, chunk is null");
  }

  auto old_x = tile->getTileX();
  auto old_y = tile->getTileY();
  auto old_z = tile->getZ();

  chunk->setTileAt(tile->getTileX() % CHUNK_SIZE, tile->getTileY() % CHUNK_SIZE,
                   tile->getZ(), nullptr);

  updateBitmaskSurround(old_x, old_y, old_z);
}

// Replace tile on map
void TileMap::replaceTile(std::shared_ptr<Tile> tile_old,
                          std::shared_ptr<Tile> tile_new) {
  if (!tile_old) {
    throw new std::runtime_error("Can not replace tile, tile_old is null");
  }

  removeTile(tile_old);
  placeTile(tile_new);
  updateBitmaskSurround(tile_new->getTileX(), tile_new->getTileY(),
                        tile_new->getZ());
}

// Check for solid tile
bool TileMap::isSolidAt(int x, int y) {
  auto tile = getTileAt(x, y, LAYER_FOREGROUND);
  return tile && tile->isSolid();
}

// Get item at position
std::shared_ptr<MapItem> TileMap::getItemAt(int x, int y) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    throw new std::runtime_error("Can not place tile, chunk is null");
  }

  return chunk->getItemAt(x, y);
}

// Place item on map
void TileMap::placeItemAt(std::shared_ptr<Item> item, int x, int y) {
  if (!item) {
    throw new std::runtime_error("Can not place item, item is null");
  }

  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    throw new std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->placeItemAt(item, x, y);
}

// Remove item from map
void TileMap::removeItem(std::shared_ptr<MapItem> item) {
  if (!item) {
    throw new std::runtime_error("Can not remove item, item is null");
  }

  auto chunk = getChunkAt(item->getX() / TILE_SIZE, item->getY() / TILE_SIZE);

  if (!chunk) {
    throw new std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->removeItem(item);
}

// Update chunks
void TileMap::tick(int x_1, int y_1, int x_2, int y_2) const {
  for (auto const& y_chunks : chunks) {
    for (auto const& chunk : y_chunks) {
      if (x_2 >= (chunk->getXIndex()) * CHUNK_SIZE * TILE_SIZE &&
          x_1 <= (chunk->getXIndex() + 1) * CHUNK_SIZE * TILE_SIZE &&
          y_2 >= (chunk->getYIndex()) * CHUNK_SIZE * TILE_SIZE &&
          y_1 <= (chunk->getYIndex() + 1) * CHUNK_SIZE * TILE_SIZE) {
        chunk->setDrawEnabled(true);
        chunk->tick();
      } else {
        chunk->setDrawEnabled(false);
      }
    }
  }
}

// Generate map
void TileMap::generateMap() {
  // Base map
  width = 10;
  height = 10;

  // Generating chunk
  std::cout << "Generating World (" << width << "," << height << ")...  ";

  // Create some chunks
  Chunk::seed = random(-10000, 10000);

  for (unsigned int t = 0; t < (unsigned)height; t++) {
    if (chunks.size() <= t) {
      chunks.push_back(std::vector<std::shared_ptr<Chunk>>());
    }

    for (int i = 0; i < width; i++) {
      chunks[t].push_back(std::make_shared<Chunk>(i, t));
    }
  }

  // Generating chunk
  std::cout << "done." << std::endl;
  std::cout << "Updating bitmasks...  ";

  // Update masks
  for (int x = 0; x < width * CHUNK_SIZE; x++) {
    for (int y = 0; y < height * CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_LAYERS; z++) {
        updateBitMask(getTileAt(x, y, z));
      }
    }
  }

  std::cout << "done." << std::endl;
}

// Clear map
void TileMap::clearMap() {
  chunks.clear();
}

// Update bitmask
void TileMap::updateBitMask(std::shared_ptr<Tile> tile) {
  if (!tile || !tile->needsBitmask()) {
    return;
  }

  unsigned char mask = 0;

  for (unsigned char i = 0; i < 4; i++) {
    const auto& [first, second] = TileMap::BITMASK_DIRECTIONS[i];

    auto current = getTileAt(tile->getTileX() + first,
                             tile->getTileY() + second, tile->getZ());

    if (current && current->getId() == tile->getId()) {
      mask += static_cast<unsigned char>(pow(2, i));
    }
  }

  tile->setMeta(mask);
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(int x, int y, int z) {
  updateBitMask(getTileAt(x, y, z));
  updateBitMask(getTileAt(x, y - 1, z));
  updateBitMask(getTileAt(x, y + 1, z));
  updateBitMask(getTileAt(x - 1, y, z));
  updateBitMask(getTileAt(x + 1, y, z));
}
