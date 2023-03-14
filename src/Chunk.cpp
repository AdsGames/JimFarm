#include "Chunk.h"

#include <iostream>
#include <sstream>

#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "utility/Tools.h"

#include "Graphics.h"

#include "SimplexNoise.h"

int Chunk::seed = 0;

Chunk::Chunk(int index_x, int index_y) : index_x(index_x), index_y(index_y) {
  generate();
}

Chunk::~Chunk() {
  for (auto const& tile : tiles) {
    if (tile) {
      Graphics::Instance()->remove(tile);
    }
  }

  for (auto const& item : items) {
    Graphics::Instance()->remove(item);
  }
}

int Chunk::getXIndex() const {
  return index_x;
}

int Chunk::getYIndex() const {
  return index_y;
}

std::string Chunk::getBiomeAt(int x, int y) const {
  auto offset = this->getTileIndex(x, y, 0);

  if (offset >= tiles.size()) {
    return "none";
  }

  std::stringstream stream;
  stream << "temp:" << temperature[offset] << " "
         << "rain:" << rainfall[offset] << " "
         << "height:" << height[offset];

  return stream.str();
}

char Chunk::getTemperatureAt(int x, int y) const {
  auto offset = this->getTileIndex(x, y, 0);

  if (offset >= tiles.size()) {
    return 0;
  }

  return temperature[offset];
}

std::shared_ptr<Tile> Chunk::getTileAt(int x, int y, int z) const {
  auto offset = this->getTileIndex(x, y, z);

  if (offset >= tiles.size()) {
    std::cout << "Tile out of bounds: " << x << ", " << y << ", " << z
              << std::endl;
    return nullptr;
  }

  return tiles[offset];
}

void Chunk::setTileAt(int x, int y, int z, std::shared_ptr<Tile> tile) {
  auto offset = this->getTileIndex(x, y, z);

  if (offset >= tiles.size()) {
    std::cout << "Tile out of bounds: " << x << ", " << y << ", " << z
              << std::endl;
    return;
  }

  if (tiles[offset]) {
    Graphics::Instance()->remove(tiles[offset]);
    tiles[offset].reset();
  }

  tiles[offset] = tile;

  if (tile && is_drawing) {
    Graphics::Instance()->add(tiles[offset]);
  }
}

// Get item at position
std::shared_ptr<MapItem> Chunk::getItemAt(int x, int y) const {
  for (auto const& i : items) {
    if (i->getX() == x * TILE_SIZE && i->getY() == y * TILE_SIZE) {
      return i;
    }
  }
  return nullptr;
}

// Place item on map
void Chunk::placeItemAt(std::shared_ptr<Item> item, int x, int y) {
  if (!item) {
    return;
  }

  // Annoying to deal with 2 coordinate systems
  auto newMapItem =
      std::make_shared<MapItem>(x * TILE_SIZE, y * TILE_SIZE, item);

  items.push_back(newMapItem);

  if (is_drawing) {
    Graphics::Instance()->add(newMapItem);
  }
}

// Remove item from map
void Chunk::removeItem(std::shared_ptr<MapItem> item) {
  if (item != nullptr) {
    for (unsigned int i = 0; i < items.size(); i++) {
      if (items.at(i) == item) {
        Graphics::Instance()->remove(items.at(i));
        items.erase(items.begin() + i);
        break;
      }
    }
  }
}

void Chunk::setDrawEnabled(bool enabled) {
  if (is_drawing == enabled) {
    return;
  }

  for (auto const& tile : tiles) {
    if (tile && enabled) {
      Graphics::Instance()->add(tile);
    } else if (tile && !enabled) {
      Graphics::Instance()->remove(tile);
    }
  }

  for (auto const& item : items) {
    if (enabled) {
      Graphics::Instance()->add(item);
    } else {
      Graphics::Instance()->remove(item);
    }
  }

  is_drawing = enabled;
}

void Chunk::tick() {
  // Tiles
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int t = 0; t < CHUNK_SIZE; t++) {
      auto offset = this->getTileIndex(i, t, LAYER_FOREGROUND);

      // Current tile
      auto& current = tiles[offset];

      if (!current) {
        continue;
      }

      // Berries
      if (current->getId() == TILE_BERRY) {
        // Grow a bit
        current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_BERRY), i, t);
          setTileAt(i, t, current->getZ(), nullptr);
        }
      }
      // Tomatos
      else if (current->getId() == TILE_TOMATO) {
        // Grow a bit
        if (!random(0, 2)) {
          current->changeMeta(1);
        }

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_TOMATO), i, t);
          setTileAt(i, t, current->getZ(), nullptr);
        }
      }
      // Carrots
      else if (current->getId() == TILE_CARROT) {
        // Grow a bit
        if (!random(0, 5)) {
          current->changeMeta(1);
        }

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_CARROT), i, t);
          setTileAt(i, t, current->getZ(), nullptr);
        }
      }
      // Lavender
      else if (current->getId() == TILE_LAVENDER) {
        // Grow a bit
        if (!random(0, 10))
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_LAVENDER), i, t);
          setTileAt(i, t, current->getZ(), nullptr);
        }
      }
    }
  }
}

void Chunk::generate() {
  this->generateBiome();

  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int t = 0; t < CHUNK_SIZE; t++) {
      const int t_x = (i + index_x * CHUNK_SIZE) * TILE_SIZE;
      const int t_y = (t + index_y * CHUNK_SIZE) * TILE_SIZE;

      const auto pos_2 = this->getTileIndex(i, t, 0);
      const auto pos_3_background = this->getTileIndex(i, t, LAYER_BACKGROUND);
      const auto pos_3_midground = this->getTileIndex(i, t, LAYER_MIDGROUND);
      const auto pos_3_foreground = this->getTileIndex(i, t, LAYER_FOREGROUND);

      // High rainfall
      // Tundra
      if (temperature[pos_2] < -32 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 30) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 2);
        }
      }
      // Forested tundra
      else if (temperature[pos_2] < 0 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 3) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(1, 2));
        }
      }
      // Forest
      else if (temperature[pos_2] < 24 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 2) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(0, 1));
        }
      }
      // Dense forest
      else if (temperature[pos_2] <= 64 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 4) != 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 3);
        }
      }

      // Low rainfall
      // Wasteland
      else if (temperature[pos_2] < -32 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
      }
      // Wasteland
      else if (temperature[pos_2] < -8 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
      }
      // Grassy wasteland
      else if (temperature[pos_2] < 0 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 8) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
        }
      }
      // Grassy wasteland
      else if (temperature[pos_2] < 24 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 1) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
          if (random(0, 100) == 0) {
            placeItemAt(std::make_shared<Item>(ITEM_CHICKEN), i, t);
          }
        }
      }
      // Savana
      else if (temperature[pos_2] < 32 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 5) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 10) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_BUSH, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 50) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_BARN, t_x, t_y, LAYER_FOREGROUND, 1);
        }
      }
      // Desert
      else if (temperature[pos_2] <= 64 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SAND, t_x, t_y, LAYER_MIDGROUND);
      }

      // Water deep
      if (height[pos_2] < -32) {
        setTileAt(i, t, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 3));
        setTileAt(i, t, LAYER_MIDGROUND, nullptr);
        setTileAt(
            i, t, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water
      else if (height[pos_2] < -19) {
        setTileAt(i, t, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 0));
        setTileAt(i, t, LAYER_MIDGROUND, nullptr);
        setTileAt(
            i, t, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water seaweed
      else if (height[pos_2] < -17) {
        setTileAt(i, t, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 1));
        setTileAt(i, t, LAYER_MIDGROUND, nullptr);
        setTileAt(
            i, t, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Shore
      else if (height[pos_2] < -14) {
        setTileAt(
            i, t, LAYER_BACKGROUND,
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(i, t, LAYER_FOREGROUND,
                  std::make_shared<Tile>(TILE_DENSE_GRASS, t_x, t_y,
                                         LAYER_FOREGROUND));
      }
      // Stone
      else if (height[pos_2] > 32) {
        setTileAt(
            i, t, LAYER_BACKGROUND,
            std::make_shared<Tile>(TILE_STONE, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(
            i, t, LAYER_MIDGROUND,
            std::make_shared<Tile>(TILE_STONE, t_x, t_y, LAYER_MIDGROUND));
        setTileAt(
            i, t, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_STONE_WALL, t_x, t_y, LAYER_FOREGROUND,
                                   (height[pos_2] - 32) / 6));
      }
    }
  }
}

void Chunk::generateBiome() {
  // Height
  auto sn_h = std::make_unique<SimplexNoise>(1.0f, 1.0f, 2.0f, 0.47f);

  // Rainfall
  auto sn_r = std::make_unique<SimplexNoise>(0.3f, 0.3f, 1.0f, 2.0f);

  // Temperature
  auto sn_t = std::make_unique<SimplexNoise>(0.3f, 0.3f, 1.0f, 2.0f);

  auto temp_size = static_cast<float>(Chunk::seed % 897);
  auto rain_size = static_cast<float>(Chunk::seed % 477);
  auto height_size = static_cast<float>(Chunk::seed);

  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int t = 0; t < CHUNK_SIZE; t++) {
      auto pos_2 = this->getTileIndex(i, t, 0);
      auto fractal_x = static_cast<float>(i + index_x * CHUNK_SIZE) / 100.0f;
      auto fractal_y = static_cast<float>(t + index_y * CHUNK_SIZE) / 100.0f;

      auto temp_val =
          sn_t->fractal(10, temp_size + fractal_x, temp_size + fractal_y) * 64;
      auto rain_val =
          sn_r->fractal(10, rain_size + fractal_x, rain_size + fractal_y) * 64;
      auto height_val =
          sn_h->fractal(10, height_size + fractal_x, height_size + fractal_y) *
          64;

      this->temperature[pos_2] = static_cast<char>(temp_val);
      this->rainfall[pos_2] = static_cast<char>(rain_val);
      this->height[pos_2] = static_cast<char>(height_val);
    }
  }
}

size_t Chunk::getTileIndex(unsigned int x,
                           unsigned int y,
                           unsigned int z) const {
  return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}