#include "TileMap.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>

#include "Graphics.h"
#include "Item.h"
#include "manager/TileTypeManager.h"
#include "behaviours/TileBehaviour.h"
#include "utility/Tools.h"

constexpr const char* STRUCTURE_PART = "tile:structure_part";

const std::array<std::pair<int, int>, 4> TileMap::BITMASK_DIRECTIONS = {
    {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

// Size
int TileMap::getWidth() const {
  return size.x * CHUNK_SIZE;
}

int TileMap::getHeight() const {
  return size.y * CHUNK_SIZE;
}

// Chunk lookup
std::shared_ptr<Chunk> TileMap::getChunkAt(const asw::Vec2i& pos) {
  // Integer division truncates toward zero, so -1 would map into chunk 0
  if (pos.x < 0 || pos.y < 0) {
    return nullptr;
  }

  auto offset_x = pos.x / CHUNK_SIZE;
  auto offset_y = pos.y / CHUNK_SIZE;

  if (offset_y >= chunks.size()) {
    return nullptr;
  }

  if (offset_x >= chunks[offset_y].size()) {
    return nullptr;
  }

  return chunks.at(offset_y).at(offset_x);
}

std::string TileMap::getBiomeAt(const asw::Vec2i& pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return "none";
  }
  return chunk->getBiomeAt(pos % CHUNK_SIZE);
}

char TileMap::getTemperatureAt(const asw::Vec2i& pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return 0;
  }

  return chunk->getTemperatureAt(pos % CHUNK_SIZE);
}

// Get tile at position
std::shared_ptr<Tile> TileMap::getTileAt(const asw::Vec2i& pos, int layer) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return nullptr;
  }

  return chunk->getTileAt(pos % CHUNK_SIZE, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw std::runtime_error("Can not place tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTilePosition());

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->setTileAt(tile->getTilePosition() % CHUNK_SIZE, tile->getZ(), tile);

  updateBitmaskSurround(tile->getTilePosition(), tile->getZ());
}

// Remove tile from map
void TileMap::removeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw std::runtime_error("Can not remove tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTilePosition());

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  auto old_pos = tile->getTilePosition();
  auto old_z = tile->getZ();

  chunk->setTileAt(tile->getTilePosition() % CHUNK_SIZE, tile->getZ(), nullptr);

  updateBitmaskSurround(old_pos, old_z);
}

// Check for solid tile
bool TileMap::isSolidAt(const asw::Vec2i& pos) {
  auto tile = getTileAt(pos, LAYER_FOREGROUND);
  return tile && tile->getType().getAttribute();
}

// Get item at position
std::shared_ptr<MapItem> TileMap::getItemAt(const asw::Vec2i& pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  return chunk->getItemAt(pos);
}

// Place item on map
void TileMap::placeItemAt(std::shared_ptr<Item> item, const asw::Vec2i& pos) {
  if (!item) {
    throw std::runtime_error("Can not place item, item is null");
  }

  auto chunk = getChunkAt(pos);

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->placeItemAt(item, pos);
}

// Remove item from map
void TileMap::removeItem(std::shared_ptr<MapItem> item) {
  if (!item) {
    throw std::runtime_error("Can not remove item, item is null");
  }

  auto chunk = getChunkAt(item->getPosition() / TILE_SIZE);

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->removeItem(item);
}

bool TileMap::inBounds(const asw::Vec2i& pos) const {
  return pos.x >= 0 && pos.y >= 0 && pos.x < getWidth() && pos.y < getHeight();
}

void TileMap::replaceTile(const std::shared_ptr<Tile>& tile,
                          const std::string& id,
                          unsigned char meta) {
  const auto pos = tile->getPosition();
  const auto z = static_cast<int>(tile->getZ());

  removeTile(tile);

  if (!id.empty()) {
    placeTile(std::make_shared<Tile>(id, pos, z, meta));
  }
}

void TileMap::placeStructure(const std::string& id, const asw::Vec2i& pos) {
  const auto& type = TileTypeManager::getTile(id);
  const int width = type.getImageWidth();

  for (int dx = 0; dx < width; dx++) {
    const auto at = pos + asw::Vec2i(dx, 0);
    if (auto existing = getTileAt(at, LAYER_FOREGROUND)) {
      removeTile(existing);
    }

    placeTile(std::make_shared<Tile>(dx == 0 ? id : STRUCTURE_PART,
                                     at * TILE_SIZE, LAYER_FOREGROUND,
                                     static_cast<unsigned char>(dx)));
  }
}

std::shared_ptr<Tile> TileMap::resolveStructure(
    const std::shared_ptr<Tile>& tile) {
  if (!tile || tile->getType().getId() != STRUCTURE_PART) {
    return tile;
  }

  return getTileAt(tile->getTilePosition() - asw::Vec2i(tile->getMeta(), 0),
                   LAYER_FOREGROUND);
}

std::vector<std::shared_ptr<Tile>> TileMap::collectTiles(
    bool visible_only,
    bool ticking_only) const {
  std::vector<std::shared_ptr<Tile>> found;

  for (auto const& y_chunks : chunks) {
    for (auto const& chunk : y_chunks) {
      if (visible_only && !chunk->isDrawing()) {
        continue;
      }

      // Top layer first, so crops see watered soil before it dries
      const auto& tiles = chunk->getTiles();
      for (int z = CHUNK_LAYERS - 1; z >= 0; z--) {
        for (unsigned int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
          const auto& tile = tiles[i + z * CHUNK_SIZE * CHUNK_SIZE];
          if (!tile) {
            continue;
          }

          const auto& behaviours = tile->getType().getBehaviours();
          if (behaviours.empty()) {
            continue;
          }

          if (ticking_only &&
              std::ranges::none_of(behaviours,
                                   [](auto const& b) { return b->ticks(); })) {
            continue;
          }

          found.push_back(tile);
        }
      }
    }
  }

  return found;
}

// Update chunks
void TileMap::tick(const Camera& camera, World& world) {
  const auto& bounds = camera.getBounds();

  // Chunks near the camera are drawn and ticked
  for (auto const& y_chunks : chunks) {
    for (auto const& chunk : y_chunks) {
      const int x_1 = chunk->getXIndex() * CHUNK_SIZE * TILE_SIZE;
      const int y_1 = chunk->getYIndex() * CHUNK_SIZE * TILE_SIZE;
      const int x_2 = x_1 + CHUNK_SIZE * TILE_SIZE;
      const int y_2 = y_1 + CHUNK_SIZE * TILE_SIZE;

      chunk->setDrawEnabled(bounds.x_2 >= x_1 && bounds.x_1 <= x_2 &&
                            bounds.y_2 >= y_1 && bounds.y_1 <= y_2);
    }
  }

  for (auto const& tile : collectTiles(true, true)) {
    // Skip tiles an earlier behaviour removed this tick
    if (getTileAt(tile->getTilePosition(), static_cast<int>(tile->getZ())) !=
        tile) {
      continue;
    }

    for (auto const& behaviour : tile->getType().getBehaviours()) {
      if (behaviour->ticks()) {
        behaviour->onTick(world, tile);
      }
    }
  }
}

void TileMap::dayEnd(World& world) {
  for (auto const& tile : collectTiles(false, false)) {
    if (getTileAt(tile->getTilePosition(), static_cast<int>(tile->getZ())) !=
        tile) {
      continue;
    }

    for (auto const& behaviour : tile->getType().getBehaviours()) {
      behaviour->onDayEnd(world, tile);
    }
  }
}

void TileMap::replaceAll(const std::string& from, const std::string& to) {
  for (int x = 0; x < getWidth(); x++) {
    for (int y = 0; y < getHeight(); y++) {
      for (unsigned int z = 0; z < CHUNK_LAYERS; z++) {
        auto tile = getTileAt(asw::Vec2i(x, y), z);
        if (tile && tile->getType().getId() == from) {
          replaceTile(tile, to);
        }
      }
    }
  }
}

void TileMap::clearItems(const asw::Vec2i& from, const asw::Vec2i& to) {
  for (int x = from.x; x <= to.x; x++) {
    for (int y = from.y; y <= to.y; y++) {
      if (!inBounds(asw::Vec2i(x, y))) {
        continue;
      }

      while (auto item = getItemAt(asw::Vec2i(x, y))) {
        removeItem(item);
      }
    }
  }
}

// Generate map
void TileMap::generateMap(const asw::Vec2<unsigned int>& size) {
  this->size = size;

  // Generating chunk
  asw::log::info("Generating World ({}, {})...  ", size.x, size.y);

  // Create some chunks
  Chunk::seed = random(-10000, 10000);

  for (unsigned int t = 0; t < size.y; t++) {
    if (chunks.size() <= t) {
      chunks.emplace_back();
    }

    for (unsigned int i = 0; i < size.x; i++) {
      chunks[t].push_back(std::make_shared<Chunk>(i, t));
    }
  }

  // Generating chunk
  asw::log::info("done.");
  asw::log::info("Updating bitmasks...  ");

  // Update masks
  for (unsigned int x = 0; x < size.x * CHUNK_SIZE; x++) {
    for (unsigned int y = 0; y < size.y * CHUNK_SIZE; y++) {
      for (unsigned int z = 0; z < CHUNK_LAYERS; z++) {
        updateBitMask(getTileAt(asw::Vec2i(x, y), z));
      }
    }
  }

  asw::log::info("done.");
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

    auto current = getTileAt(
        tile->getTilePosition() + asw::Vec2i(first, second), tile->getZ());

    if (current && current->getType().getBitmaskGroup() ==
                       tile->getType().getBitmaskGroup()) {
      mask += static_cast<unsigned char>(pow(2, i));
    }
  }

  tile->setMeta(mask);
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(const asw::Vec2i& pos, int z) {
  updateBitMask(getTileAt(pos + asw::Vec2i(0, 0), z));
  updateBitMask(getTileAt(pos + asw::Vec2i(0, -1), z));
  updateBitMask(getTileAt(pos + asw::Vec2i(0, 1), z));
  updateBitMask(getTileAt(pos + asw::Vec2i(-1, 0), z));
  updateBitMask(getTileAt(pos + asw::Vec2i(1, 0), z));
}

nlohmann::json TileMap::toJson() const {
  // Tile ids are stored once in a palette, cells hold palette index + 1
  std::vector<std::string> palette;
  std::map<std::string, int> palette_index;

  auto indexOf = [&](const std::string& id) {
    auto found = palette_index.find(id);
    if (found != palette_index.end()) {
      return found->second;
    }
    palette.push_back(id);
    palette_index[id] = static_cast<int>(palette.size());
    return static_cast<int>(palette.size());
  };

  nlohmann::json json_chunks = nlohmann::json::array();

  for (auto const& y_chunks : chunks) {
    for (auto const& chunk : y_chunks) {
      std::vector<int> ids;
      std::vector<int> metas;

      for (auto const& tile : chunk->getTiles()) {
        ids.push_back(tile ? indexOf(tile->getType().getId()) : 0);
        metas.push_back(tile ? tile->getMeta() : 0);
      }

      nlohmann::json items = nlohmann::json::array();
      for (auto const& item : chunk->getItems()) {
        const auto pos = item->getPosition() / TILE_SIZE;
        items.push_back({item->itemPtr->getType().getId(),
                         item->itemPtr->getMeta(), pos.x, pos.y});
      }

      json_chunks.push_back({{"x", chunk->getXIndex()},
                             {"y", chunk->getYIndex()},
                             {"tiles", ids},
                             {"meta", metas},
                             {"items", items}});
    }
  }

  return {{"seed", Chunk::seed},
          {"size", {size.x, size.y}},
          {"palette", palette},
          {"chunks", json_chunks}};
}

void TileMap::fromJson(const nlohmann::json& data) {
  clearMap();

  Chunk::seed = data["seed"];
  size = asw::Vec2<unsigned int>(data["size"][0], data["size"][1]);

  const auto palette = data["palette"].get<std::vector<std::string>>();

  for (unsigned int t = 0; t < size.y; t++) {
    chunks.emplace_back();
    for (unsigned int i = 0; i < size.x; i++) {
      chunks[t].push_back(std::make_shared<Chunk>(i, t, false));
    }
  }

  for (auto const& json_chunk : data["chunks"]) {
    auto chunk = chunks.at(json_chunk["y"]).at(json_chunk["x"]);
    const auto& ids = json_chunk["tiles"];
    const auto& metas = json_chunk["meta"];

    for (unsigned int z = 0; z < CHUNK_LAYERS; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
        for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
          const auto index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
          const int id = ids[index];
          if (id == 0) {
            continue;
          }

          const auto tile_pos =
              asw::Vec2i(x + chunk->getXIndex() * CHUNK_SIZE,
                         y + chunk->getYIndex() * CHUNK_SIZE);

          chunk->setTileAt(
              asw::Vec2i(x, y), z,
              std::make_shared<Tile>(palette.at(id - 1), tile_pos * TILE_SIZE,
                                     z, metas[index].get<unsigned char>()));
        }
      }
    }

    for (auto const& item : json_chunk["items"]) {
      chunk->placeItemAt(
          std::make_shared<Item>(item[0].get<std::string>(),
                                 item[1].get<unsigned char>()),
          asw::Vec2i(item[2], item[3]));
    }
  }
}
