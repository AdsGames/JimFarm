#ifndef SRC_ACTION_PARTICLES_H_
#define SRC_ACTION_PARTICLES_H_

#include <asw/asw.h>

#include <string>
#include <vector>

#include "utility/Camera.h"

// Short bursts in world space when the player does something (dirt when
// tilling, drops when watering, chips when chopping, coins when selling)
class ActionParticles {
 public:
  // Burst of a named preset centred on pos in world pixels, unknown names
  // are ignored so data files can name presets freely
  void burst(const std::string& preset, const asw::Vec2f& pos);

  void update(float dt);

  // Draw relative to the camera, into the world buffer
  void draw(const Camera& camera) const;

  void clear() { particles.clear(); }

 private:
  struct Particle {
    asw::Vec2f pos;
    asw::Vec2f vel;
    asw::Color color;
    float gravity{0.0F};
    float size{1.0F};
    float age{0.0F};
    float life{0.5F};
  };

  std::vector<Particle> particles{};
};

#endif  // SRC_ACTION_PARTICLES_H_
