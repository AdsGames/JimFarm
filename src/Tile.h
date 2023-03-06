#ifndef SRC_TILE_H_
#define SRC_TILE_H_

#include <memory>
#include <string>

#include "Sprite.h"
#include "manager/TileType.h"

const int MAX_TILE_META = 255;

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;

class Tile : public Sprite {
 public:
  // Ctor and Dtor
  Tile(char id, int x, int y, int z, unsigned char meta = 0);
  ~Tile() final = default;

  // Define < operator for sorting
  bool operator<(const Tile& other) const { return (y < other.y); }

  // Gets size
  int getWidth() const;
  int getHeight() const;

  // Drawing
  void draw(float x_1, float y_1, float x_2, float y_2) const override;
  void drawAt(int newX, int newY) const;

  // Is this type solid?
  bool isSolid() const;

  // Modify ID
  unsigned char getID() const;

  // Get name of tile
  std::string getName() const;

  // Access and set meta data byte
  void setMeta(unsigned char meta);
  unsigned char getMeta() const;
  void changeMeta(unsigned char amt);

  // Temperature and moisture

  // Get image type
  bool needsBitmask() const;

 private:
  // Metadata info
  unsigned char meta;

  // Ptr to tile type
  TileType& tile_pointer;
};

#endif  // SRC_TILE_H_
