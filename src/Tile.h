#ifndef SRC_TILE_H_
#define SRC_TILE_H_

#include <string>

#include "Sprite.h"
#include "manager/TileType.h"

#define MAX_TILE_META 255

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

class Tile : public Sprite {
 public:
  // Ctor and Dtor
  Tile(char id, int x, int y, int z, unsigned char meta = 0);
  virtual ~Tile() {}

  // Define < operator for sorting
  virtual bool operator<(const Tile& other) const { return (y < other.y); }

  // Gets size
  int getWidth();
  int getHeight();

  // Drawing
  void draw(float x_1, float y_1, float x_2, float y_2) override;
  void draw_at(int newX, int newY);

  // Is this type solid?
  bool isSolid();

  // Modify ID
  unsigned char getID();

  // Get name of tile
  std::string getName();

  // Access and set meta data byte
  void setMeta(unsigned char meta);
  unsigned char getMeta();
  void changeMeta(unsigned char amt);

  // Temperature and moisture

  // Get image type
  bool needsBitmask();

 private:
  // Metadata info
  unsigned char meta;

  // Ptr to tile type
  TileType* tile_pointer;
};

#endif  // SRC_TILE_H_
