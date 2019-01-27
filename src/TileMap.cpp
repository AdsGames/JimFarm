#include "TileMap.h"

#include <fstream>
#include <math.h>

#include "Graphics.h"
#include "Item.h"

#include "tile_defs.h"
#include "Tools.h"

TileMap::TileMap() {
  width = 1;
  height = 1;
}

TileMap::~TileMap() {
  //dtor
}

// Size
int TileMap::getWidth() {
  return width * CHUNK_WIDTH;
}

int TileMap::getHeight() {
  return height * CHUNK_HEIGHT;
}

// Chunk lookup
Chunk* TileMap::chunk_at(int x, int y) {
  for (unsigned int i = 0; i < chunks.size(); i++) {
    if (x >= (chunks.at(i) -> getX())     * CHUNK_WIDTH * TILE_WIDTH &&
        x <  (chunks.at(i) -> getX() + 1) * CHUNK_WIDTH * TILE_WIDTH &&
        y >= (chunks.at(i) -> getY())     * CHUNK_HEIGHT * TILE_HEIGHT &&
        y <  (chunks.at(i) -> getY() + 1) * CHUNK_HEIGHT * TILE_HEIGHT) {
      return chunks.at(i);
    }
  }
  return nullptr;
}

// Get tile at position
Tile* TileMap::tile_at (int positionX, int positionY, int layer) {
  Chunk* chunk = chunk_at(positionX, positionY);
  if (!chunk)
    return nullptr;
  return chunk -> get_tile_at(positionX, positionY, layer);
}

// Place tile on map (world gen)
void TileMap::place_tile (Tile* newTile) {
  if (!newTile)
    return;

  Chunk* chunk = chunk_at(newTile -> getX(), newTile -> getY());

  if (!chunk)
    return;

  return chunk -> set_tile_at(newTile -> getX(), newTile -> getY(), newTile -> getZ(), newTile);
}

// Place tile on map if there isnt one otherwise it deletes it (world gen)
bool TileMap::place_tile_safe (Tile* newTile, int opposite_layer_id) {
  Tile *layerTile = tile_at (newTile -> getX(), newTile -> getY(), newTile -> getZ());
  Tile *notLayerTile = tile_at (newTile -> getX(), newTile -> getY(), LAYER_BACKGROUND);

  if (opposite_layer_id == -1 || (notLayerTile && notLayerTile -> getID() == opposite_layer_id)) {
    if (!layerTile) {
      place_tile (newTile);
      return true;
    }
  }
  delete newTile;
  return false;
}

// Replace tile on map
void TileMap::replace_tile (Tile *oldTile, Tile *newTile) {
  if (oldTile) {
    remove_tile (oldTile);
    place_tile (newTile);

    // Update bitmasks
    update_bitmask_surround(newTile);
  }
}

// Remove tile from map
void TileMap::remove_tile (Tile* newTile) {
  /*if (newTile) {
    for (unsigned int i = 0; i < map_tiles.size(); i++) {
      if (map_tiles.at(i) == newTile) {
        Graphics::Instance() -> remove(map_tiles.at(i));
        Tile *tempPtr = map_tiles.at(i);
        map_tiles.erase (map_tiles.begin() + i);
        delete tempPtr;
        break;
      }
    }
  }*/
}

// Check for solid tile
bool TileMap::solid_at (int positionX, int positionY) {
  if (tile_at (positionX, positionY, LAYER_FOREGROUND))
    return tile_at (positionX, positionY, LAYER_FOREGROUND) -> isSolid();
  return false;
}

// Update chunks
void TileMap::update(int x_1, int y_1, int x_2, int y_2) {
  for (unsigned int i = 0; i < chunks.size(); i++) {
    chunks.at(i) -> update(x_1, y_1, x_2, y_2);
  }
}

// Generate map
void TileMap::generate_map() {
  // Base map
  width = 10;
  height = 10;

  // Create some chunks
  for (int i = 0; i < width; i++) {
    for (int t = 0; t < height; t++) {
      chunks.push_back(new Chunk(i, t));
    }
  }

  // Disable sorting
  Graphics::Instance() -> disableSort();
  const int PLACED_MULTIPLIER = getWidth() / 16 + getHeight() / 16;

  // Rivers
  int placed = 0;
  while (placed < (int)(0.25f * (float)PLACED_MULTIPLIER)) {
    int river_x_1 = random(0, getWidth() - 1);
    int river_y_1 = 0;
    int river_x_2 = random(0, getWidth() - 1);
    int river_y_2 = getHeight();
    int river_width = random(4,8);

    while (river_y_1 < river_y_2) {
      for (int i = 0; i < river_width; i++) {
        if (river_x_1 + i < getWidth()) {
          Tile *foregroundTile = tile_at((river_x_1 + i) * 16, river_y_1 * 16, LAYER_FOREGROUND);
          Tile *backgroundTile = tile_at((river_x_1 + i) * 16, river_y_1 * 16, LAYER_BACKGROUND);
          if (!foregroundTile) {
            place_tile(new Tile (TILE_WATER, (river_x_1 + i) * 16, river_y_1 * 16, LAYER_FOREGROUND));

            int underwater_meta = random(0,100);
            if (underwater_meta > 3)
              underwater_meta = 0;

            replace_tile(backgroundTile, new Tile (TILE_UNDERWATER_SOIL, (river_x_1 + i) * 16, river_y_1 * 16, LAYER_BACKGROUND, underwater_meta));
          }
        }
      }

      if (river_x_1 < getWidth() && river_x_1 > 0 && random(0, 2) == 0) {
        river_x_1 += int(river_x_1 < river_x_2) - int(river_x_1 > river_x_2);
        if (river_x_1 < getWidth()) {
          Tile *foregroundTile = tile_at(river_x_1 * 16, river_y_1 * 16, LAYER_FOREGROUND);
          Tile *backgroundTile = tile_at(river_x_1 * 16, river_y_1 * 16, LAYER_BACKGROUND);
          if (!foregroundTile) {
            place_tile(new Tile (TILE_WATER, river_x_1 * 16, river_y_1 * 16, LAYER_FOREGROUND));

            int underwater_meta = random(0,100);
            if (underwater_meta > 3)
              underwater_meta = 0;

            replace_tile(backgroundTile, new Tile (TILE_UNDERWATER_SOIL, river_x_1 * 16, river_y_1 * 16, LAYER_BACKGROUND, underwater_meta));
          }
        }
      }
      if (river_y_1 < getHeight())
        river_y_1 ++;
    }
    placed++;
  }

  // Place grass
  placed = 0;
  while (placed < 10 * PLACED_MULTIPLIER) {
    int random_x = random (0, getWidth()) * 16;
    int random_y = random (0, getHeight()) * 16;
    //Tile *backgroundTile = tile_at (random_x, random_y, BACKGROUND);
    placed += place_tile_safe (new Tile (TILE_DENSE_GRASS, random_x, random_y, LAYER_FOREGROUND, random(0, 5)), TILE_GRASS);
  }

  // Grow Grass
  placed = 0;
  /*while (placed < 100 * PLACED_MULTIPLIER) {
    for (unsigned int t = 0; t < map_tiles.size(); t++) {
      if (map_tiles.at(t) -> getID() == TILE_DENSE_GRASS && !random(0, 10)) {
        Tile *current = map_tiles.at(t);
        placed += place_tile_safe (new Tile(TILE_DENSE_GRASS, current -> getX() + random(-1, 1) * 16,
                                            current -> getY() + random(-1, 1) * 16, LAYER_FOREGROUND, current -> getMeta()),
                                            TILE_GRASS);
      }
    }
  }*/

  // Place trees
  placed = 0;
  while (placed < 20 * PLACED_MULTIPLIER) {
    int random_x = random (0, getWidth()) * 16;
    int random_y = random (0, getHeight()) * 16;
    //Tile *backgroundTile = tile_at (random_x, random_y, FOREGROUND);
    placed += place_tile_safe (new Tile (TILE_TREE, random_x, random_y, LAYER_FOREGROUND, random(0,3)), TILE_GRASS);
  }

  // Grow trees
  placed = 0;
  /*while (placed < 90 * PLACED_MULTIPLIER)
    for (unsigned int t = 0; t < map_tiles.size(); t++)
      if (map_tiles.at(t) -> getID() == TILE_TREE && !random(0, 10))
        placed += place_tile_safe (new Tile(TILE_TREE,
                                            map_tiles.at(t) -> getX() + random(-1, 1) * 16,
                                            map_tiles.at(t) -> getY() + random(-1, 1) * 16,
                                            LAYER_FOREGROUND, random(0,3)),
                                            TILE_GRASS);*/

  // Place chickens
  /*placed = 0;
  while (placed < 3 * PLACED_MULTIPLIER) {
    int random_x = random (0, getWidth()) * 16;
    int random_y = random (0, getHeight()) * 16;
    Tile *foregroundTile = tile_at (random_x, random_y, LAYER_FOREGROUND);
    if (foregroundTile && foregroundTile -> getID() == TILE_DENSE_GRASS) {
      place_item (new Item (ITEM_CHICKEN), random_x, random_y);
      placed ++;
    }
  }

  // Place sticks
  placed = 0;
  while (placed < 10 * PLACED_MULTIPLIER) {
    int random_x = random (0, getWidth()) * 16;
    int random_y = random (0, getHeight()) * 16;
    if(!solid_at (random_x, random_y)) {
      place_item (new Item (ITEM_STICK), random_x, random_y );
      placed ++;
    }
  }

  // Place items
  place_item (new Item(ITEM_HOE), 17 * 16, 5 * 16);
  place_item (new Item(ITEM_CARROT_SEED), 18 * 16, 5 * 16);
  place_item (new Item(ITEM_LAVENDER_SEED), 19 * 16, 5 * 16);
  place_item (new Item(ITEM_BERRY_SEED), 20 * 16, 5 * 16);
  place_item (new Item(ITEM_TOMATO_SEED), 21 * 16, 5 * 16);
  place_item (new Item(ITEM_WATERING_CAN), 17 * 16, 6 * 16);
  place_item (new Item(ITEM_AXE), 18 * 16, 6 * 16);
  place_item (new Item(ITEM_SCYTHE), 19 * 16, 6 * 16);
  place_item (new Item(ITEM_SHOVEL), 20 * 16, 6 * 16);*/

  // Update masks
  for (int x = 0; x < width * CHUNK_WIDTH; x++) {
    for (int y = 0; y < height * CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LAYERS; z++) {
        update_bitmask (tile_at(x * TILE_WIDTH, y * TILE_HEIGHT, z));
      }
    }
  }

  // Sort
  Graphics::Instance() -> enableSort();
}

// Manually load new file
void TileMap::load_map (std::string fileName) {
  //Change size
  //std::string fileLoad = fileName + ".txt";
  //std::ifstream findSize(fileLoad.c_str());

  //width = 64;
  //height = 64;

  /*int data;
  while (findSize >> data) {
    if(height == 0)
      width++;
    if(findSize.peek() == '\n')
      height++;
  }

  //Setup Map
  if(fileName != "") {
    fileLoad = fileName + ".txt";
    std::ifstream read(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for (int t = 0; t < height; t++) {
      for (int i = 0; i < width; i++) {
        int newTileType;
        read >> newTileType;
        Tile* newTile = new Tile (newTileType, i * 16, t * 16);
        map_tiles.push_back (newTile);
      }
    }
    read.close();

    fileLoad = fileName + "_back.txt";
    std::ifstream read2(fileLoad.c_str());
    std::cout << "Loading " << fileLoad << "\n";

    for (int t = 0; t < height; t++) {
      for (int i = 0; i < width; i++) {
        int newTileType;
        read2 >> newTileType;
        if (newTileType != TILE_NULL) {
          Tile* newTile = new Tile (newTileType, i * 16, t * 16);
          map_tiles_foreground.push_back (newTile);
        }
      }
    }
    read2.close();
  }*/
}

// Clear map
void TileMap::clear_map() {
  /*for (std::vector<Tile*>::iterator i = map_tiles.begin(); i != map_tiles.end(); ++i) {
    delete *i;
  }*/

  //map_tiles.clear();
}


// Update bitmask
void TileMap::update_bitmask (Tile *newTile, bool layer) {
  if (newTile && newTile -> needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, newTile -> getZ());
      if (current && current -> getID() == newTile -> getID())
        mask += pow (2, i);
    }

    newTile -> setMeta (mask);
  }
}

// Update bitmask (and neighbours)
void TileMap::update_bitmask_surround (Tile *newTile, bool layer) {
  if (newTile) {
    update_bitmask (newTile);
    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, layer);
      if (current)
        update_bitmask (current);
    }
  }
}
