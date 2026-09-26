/*
  Tile Type
  Allan Legemaate
  24/11/15
  Defenitions of the tiles ingame
*/
#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include <asw/asw.h>
#include <array>
#include <memory>
#include <string>
#include <vector>

class TileBehaviour;

enum class ImageType {
  Static,
  MetaMap,
  MetaMap2,
  Animated,
  Dynamic,
  None,
};

class TileType {
 public:
  TileType() = default;
  TileType(unsigned char width,
           unsigned char height,
           const std::string& id,
           const std::string& name,
           unsigned char attribute = 0);

  // Get type
  const std::string& getId() const { return id; }

  // Get name
  const std::string& getName() const { return name; }

  // Get type
  unsigned char getAttribute() const { return attribute; }

  // Tex
  unsigned char getImageX() const { return image_cord_x; }
  unsigned char getImageY() const { return image_cord_y; }
  unsigned char getWidth() const { return width; }
  unsigned char getHeight() const { return height; }

  // Sprite size in tiles
  unsigned char getImageWidth() const { return image_w; }
  unsigned char getImageHeight() const { return image_h; }

  // Draw
  void draw(int x, int y, unsigned char meta = 0) const;

  // Set sprite sheet
  void setSpriteSheet(asw::Texture spriteSheet);

  // Set special image stuff
  void setImageType(const std::string& imageType,
                    unsigned char sheetWidth,
                    unsigned char sheetHeight,
                    unsigned char imageX,
                    unsigned char imageY,
                    unsigned char imageWidth,
                    unsigned char imageHeight);

  // Set behaviours
  void attachBehaviour(std::shared_ptr<TileBehaviour> behaviour);

  const std::vector<std::shared_ptr<TileBehaviour>>& getBehaviours() const {
    return behaviours;
  }

  ImageType getImageType() const { return this->image_type; }

  // Colour multiplied into the sprite, used to reuse art
  void setTint(const asw::Color& color) {
    tint = color;
    has_tint = true;
  }

  // Living tiles (animals) bob gently when drawn
  void setBobs(bool value) { bobs = value; }
  bool getBobs() const { return bobs; }

  // Tiles in the same group join when bitmasking, defaults to id
  const std::string& getBitmaskGroup() const {
    return bitmask_group.empty() ? id : bitmask_group;
  }
  void setBitmaskGroup(const std::string& group) { bitmask_group = group; }

 private:
  std::string id{};

  unsigned char width{};
  unsigned char height{};

  std::string name{""};
  unsigned char attribute{};

  unsigned char num_images{0};

  ImageType image_type{ImageType::Static};
  unsigned char sheet_width{1};
  unsigned char sheet_height{1};

  unsigned char image_cord_x{0};
  unsigned char image_cord_y{0};

  unsigned char image_h{1};
  unsigned char image_w{1};

  asw::Texture sprite_sheet{nullptr};

  std::vector<std::shared_ptr<TileBehaviour>> behaviours{};

  asw::Color tint{255, 255, 255, 255};
  bool has_tint{false};

  bool bobs{false};

  std::string bitmask_group{};
};

#endif  // TILE_TYPE_H
