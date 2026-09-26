#ifndef SRC_CREATURE_H_
#define SRC_CREATURE_H_

#include <asw/asw.h>

#include "Sprite.h"

class World;

// Hostile night creature (wolf). Hunts the player and their chickens,
// keeps away from campfires and leaves at dawn.
class Creature : public Sprite {
 public:
  explicit Creature(const asw::Vec2i& pos);

  void draw(const Camera& camera) const override;

  void update(World& world, const asw::Vec2i& player_pos, float dt);

  // Take damage, knocked away from a point. True when it dies.
  bool hit(World& world, int damage, const asw::Vec2i& from);

  // Should be removed from the world
  bool isGone() const { return gone; }

  // Tile under the centre of the sprite
  asw::Vec2i getTile() const;

  static void loadImages();

 private:
  // Move with tile collision, false when blocked on both axes
  bool move(World& world, const asw::Vec2f& delta);
  bool blocked(World& world, const asw::Vec2f& at) const;

  // Nearest chicken within range, or false
  bool findPrey(World& world, asw::Vec2i& prey) const;

  asw::Vec2f fpos{};
  asw::Vec2f wander_dir{};
  float wander_timer{0.0F};
  float attack_cooldown{0.0F};
  float flee_timer{0.0F};
  float hurt_timer{0.0F};
  float anim{0.0F};
  int hp{3};
  bool moving{false};
  bool leaving{false};
  bool gone{false};

  static asw::Texture sheet;
};

#endif  // SRC_CREATURE_H_
