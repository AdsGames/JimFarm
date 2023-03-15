#ifndef SRC_TILE_H_
#define SRC_TILE_H_

#include <memory>
#include <string>

#include "Sprite.h"
#include "manager/TileType.h"
#include "utility/Camera.h"

constexpr int MAX_TILE_META = 255;

constexpr int TILE_SIZE = 16;

class Tile : public Sprite {
 public:
  Tile(const std::string& id, int x, int y, int z, unsigned char meta = 0);
  ~Tile() final = default;

  // Gets size
  int getWidth() const;
  int getHeight() const;

  int getTileX() const;
  int getTileY() const;

  // Drawing
  void draw(const Camera& camera) const override;

  // Is this type solid?
  bool isSolid() const;

  // Modify ID
  const std::string& getId() const;

  // Get name of tile
  std::string getName() const;

  // Access and set meta data byte
  void setMeta(unsigned char meta);
  unsigned char getMeta() const;
  void changeMeta(unsigned char amt);

  // Get image type
  bool needsBitmask() const;

 private:
  // Metadata info
  unsigned char meta;

  // Ptr to tile type
  TileType& tile_pointer;
};

#endif  // SRC_TILE_H_
