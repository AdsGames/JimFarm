#ifndef SRC_FLOATING_TEXT_H_
#define SRC_FLOATING_TEXT_H_

#include <asw/asw.h>

#include <string>
#include <vector>

#include "utility/Camera.h"

// Short lived text in world space (damage numbers) that rises and fades
class FloatingTexts {
 public:
  // Centre of the text in world pixels
  void add(const std::string& text,
           const asw::Vec2f& pos,
           const asw::Color& color);

  void update(float dt);

  // Draw relative to the camera, into the world buffer
  void draw(const asw::Font& font, const Camera& camera) const;

  void clear() { texts.clear(); }

 private:
  struct Entry {
    std::string text;
    asw::Vec2f pos;
    asw::Color color;
    float age{0.0F};
  };

  std::vector<Entry> texts{};
};

#endif  // SRC_FLOATING_TEXT_H_
