#ifndef SRC_CREATURE_H_
#define SRC_CREATURE_H_

#include <asw/asw.h>

#include "Sprite.h"

class World;

enum class CreatureKind { Wolf, Deer, Rabbit };

// Wolves hunt the player and their chickens at night, keep away from
// campfires and leave at dawn. Deer and rabbits roam by day and run from the
// player.
class Creature : public Sprite {
 public:
  explicit Creature(const asw::Vec2i& pos,
                    CreatureKind kind = CreatureKind::Wolf);

  CreatureKind getKind() const { return kind; }
  bool isHostile() const { return kind == CreatureKind::Wolf; }

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

  // Wolf hunting, and deer and rabbits grazing
  void updateHunter(World& world, const asw::Vec2f& to_player, float dt);
  void updateGrazer(World& world, const asw::Vec2f& to_player, float dt);

  void wander(World& world, float speed, float dt);

  CreatureKind kind{CreatureKind::Wolf};

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
