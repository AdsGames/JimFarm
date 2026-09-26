#include "TileType.h"

#include <math.h>
#include <algorithm>
#include <array>
#include <cmath>

#include "../GameState.h"

namespace {
// Multiply colours per season, index matches Season
const std::array<asw::Color, SEASONS_PER_YEAR> SEASON_TINTS{
    asw::Color(235, 255, 235),  // Spring, fresh green
    asw::Color(255, 250, 200),  // Summer, slightly yellow
    asw::Color(255, 185, 120),  // Fall, orange
    asw::Color(215, 225, 235),  // Winter, pale grey
};

// Days at the end of a season spent blending into the next
constexpr float SEASON_BLEND_DAYS = 2.0F;

// Wind sway
constexpr float SWAY_SPEED = 1.8F;
constexpr float SWAY_PHASE_PER_TILE = 0.7F;
constexpr float SWAY_AMPLITUDE = 1.6F;

unsigned char lerp(unsigned char a, unsigned char b, float t) {
  return static_cast<unsigned char>(std::lround(a + (b - a) * t));
}

unsigned char multiply(unsigned char a, unsigned char b) {
  return static_cast<unsigned char>(a * b / 255);
}
}  // namespace

asw::Color TileType::season_tint{255, 255, 255, 255};
float TileType::wind_time{0.0F};

void TileType::updateEnvironment(float dt, const GameState& state) {
  wind_time = std::fmod(wind_time + dt, 1000.0F);

  // Progress through the current day, 0 to 1
  const float day_progress =
      std::clamp((state.getMinutes() - DAY_START_MINUTES) /
                     static_cast<float>(DAY_END_MINUTES - DAY_START_MINUTES),
                 0.0F, 1.0F);

  // Blend toward the next season over its final days
  const float days_left = static_cast<float>(DAYS_PER_SEASON - state.getDay()) +
                          1.0F - day_progress;
  const float t = std::clamp(1.0F - days_left / SEASON_BLEND_DAYS, 0.0F, 1.0F);

  const auto index = static_cast<int>(state.getSeason());
  const auto& from = SEASON_TINTS[index];
  const auto& to = SEASON_TINTS[(index + 1) % SEASONS_PER_YEAR];

  season_tint = asw::Color(lerp(from.r, to.r, t), lerp(from.g, to.g, t),
                           lerp(from.b, to.b, t));
}

// Init tile
TileType::TileType(unsigned char width,
                   unsigned char height,
                   const std::string& id,
                   const std::string& name,
                   unsigned char attribute)
    : id(id), width(width), height(height), name(name), attribute(attribute) {}

// Draw tile
void TileType::draw(int x, int y, unsigned char meta, int tile_x) const {
  if (image_type == ImageType::None) {
    return;
  }

  int image_index = 0;
  auto h_px = image_h * 16;
  auto w_px = image_w * 16;
  auto i_x_px = image_cord_x * 16;
  auto i_y_px = image_cord_y * 16;

  if (image_type == ImageType::MetaMap || image_type == ImageType::Animated) {
    image_index = num_images * meta / 256;
    i_x_px += (image_index % sheet_width) * 16;
    i_y_px += (image_index / sheet_width) * 16;

  } else if (image_type == ImageType::MetaMap2 ||
             image_type == ImageType::Dynamic) {
    image_index = meta % num_images;
    i_x_px += (image_index % sheet_width) * 16;
    i_y_px += (image_index / sheet_width) * 16;
  }

  // Combine art tint with the season colour
  const bool tinted = has_tint || seasonal;
  if (tinted) {
    auto color = has_tint ? tint : asw::color::white;
    if (seasonal) {
      color = asw::Color(multiply(color.r, season_tint.r),
                         multiply(color.g, season_tint.g),
                         multiply(color.b, season_tint.b));
    }
    asw::draw::set_tint(sprite_sheet, color);
  }

  // Wind sway, whole pixels to keep the art crisp
  const int sway_px =
      sway ? static_cast<int>(std::lround(
                 std::sin(wind_time * SWAY_SPEED +
                          static_cast<float>(tile_x) * SWAY_PHASE_PER_TILE) *
                 SWAY_AMPLITUDE))
           : 0;

  if (sway_px != 0 && h_px > 16) {
    // Tall sprites keep their base row planted, only the top sways
    const auto top_h = h_px - 16;
    asw::draw::stretch_sprite_blit(
        sprite_sheet, asw::Quadf(i_x_px, i_y_px, w_px, top_h),
        asw::Quadf(x + sway_px, y - h_px + 16, w_px, top_h));
    asw::draw::stretch_sprite_blit(sprite_sheet,
                                   asw::Quadf(i_x_px, i_y_px + top_h, w_px, 16),
                                   asw::Quadf(x, y, w_px, 16));
  } else {
    asw::draw::stretch_sprite_blit(
        sprite_sheet, asw::Quadf(i_x_px, i_y_px, w_px, h_px),
        asw::Quadf(x + sway_px, y - h_px + 16, w_px, h_px));
  }

  if (tinted) {
    asw::draw::set_tint(sprite_sheet, asw::color::white);
  }
}

// Give a sprite sheet to this tile
void TileType::setSpriteSheet(asw::Texture newSpriteSheet) {
  sprite_sheet = newSpriteSheet;
}

// Set special image stuff
void TileType::setImageType(const std::string& type,
                            unsigned char sheet_w,
                            unsigned char sheet_h,
                            unsigned char image_x,
                            unsigned char image_y,
                            unsigned char image_width,
                            unsigned char image_height) {
  // Default, dynamic or animated
  if (type == "meta_map") {
    image_type = ImageType::MetaMap;
  } else if (type == "meta_map_2") {
    image_type = ImageType::MetaMap2;
  } else if (type == "animated") {
    image_type = ImageType::Animated;
  } else if (type == "dynamic") {
    image_type = ImageType::Dynamic;
  } else if (type == "none") {
    image_type = ImageType::None;
  } else {
    image_type = ImageType::Static;
  }
  sheet_width = sheet_w;
  sheet_height = sheet_h;
  image_w = image_width;
  image_h = image_height;
  image_cord_x = image_x;
  image_cord_y = image_y;

  num_images = sheet_width * sheet_height;
}

void TileType::attachBehaviour(std::shared_ptr<TileBehaviour> behaviour) {
  behaviours.push_back(behaviour);
}