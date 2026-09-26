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
  Tile(const std::string& id,
       const asw::Vec2i& pos,
       int z,
       unsigned char meta = 0);

  const asw::Vec2i getTilePosition() const;

  // Drawing
  void draw(const Camera& camera) const override;

  const TileType& getType() const;

  // Access and set meta data byte
  void setMeta(unsigned char meta);

  unsigned char getMeta() const;

  void changeMeta(unsigned char amt);

  void damage(unsigned char amt);

  // Get image type
  bool needsBitmask() const;

 private:
  // Metadata info
  unsigned char meta;

  // Hitpoints
  unsigned char hitpoints{255};

  // Ptr to tile type
  TileType& tile_pointer;
};

#endif  // SRC_TILE_H_
