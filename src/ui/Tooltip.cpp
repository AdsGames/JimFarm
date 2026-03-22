#include "Tooltip.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <vector>

#include "../Item.h"
#include "../manager/ItemTypeManager.h"
#include "UiScale.h"

namespace {
constexpr int PADDING = 3;
constexpr int LINE_HEIGHT = 9;

std::vector<std::string> lines{};

asw::Font& font() {
  static asw::Font loaded = asw::assets::load_font(
      "assets/fonts/pixelart.ttf", 8, asw::FontStyle::Pixel);
  return loaded;
}

std::string capitalise(std::string text) {
  if (!text.empty()) {
    text[0] = static_cast<char>(std::toupper(text[0]));
  }
  return text;
}
}  // namespace

void Tooltip::setItem(const Item& item, int price) {
  const auto& id = item.getType().getId();
  const auto& info = ItemTypeManager::getInfo(id);

  lines = {capitalise(item.getType().getName())};

  if (price > 0) {
    lines.push_back(std::format("Costs ${}", price));
  } else if (info.value > 0) {
    lines.push_back(std::format("Sells for ${}", info.value));
  }

  if (info.edible) {
    std::string food;
    if (info.hunger > 0) {
      food += std::format("Food +{:.0f} ", info.hunger);
    }
    if (info.thirst > 0) {
      food += std::format("Water +{:.0f} ", info.thirst);
    }
    if (info.health != 0) {
      food += std::format("HP {:+.0f} ", info.health);
    }
    if (info.warmth > 0) {
      food += std::format("Warm +{:.0f}", info.warmth);
    }
    if (!food.empty()) {
      lines.push_back(food);
    }
  }

  // Containers such as the watering can keep their charge in meta
  if (info.start_meta > 0) {
    lines.push_back(std::format("{}/{} left", item.getMeta(), info.start_meta));
  }

  if (info.attack > 0) {
    lines.push_back(std::format("Attack {}", info.attack));
  }

  if (info.carry_warmth > 0) {
    lines.push_back(std::format("Warm +{:.0f}C while carried",
                                info.carry_warmth));
  }
}

void Tooltip::draw(const asw::Vec2i& ui_size) {
  if (lines.empty()) {
    return;
  }

  int width = 0;
  for (auto const& line : lines) {
    width = std::max(width, asw::util::get_text_size(font(), line).x);
  }
  width += PADDING * 2;
  const int height = static_cast<int>(lines.size()) * LINE_HEIGHT + PADDING * 2;

  // Beside the cursor, kept on screen
  const auto mouse = getUiMouse();
  const int x = std::clamp(mouse.x + 10, 0, ui_size.x - width);
  const int y = std::clamp(mouse.y + 10, 0, ui_size.y - height);

  asw::draw::rect_fill(asw::Quadf(x, y, width, height),
                       asw::Color(20, 20, 30, 230));
  asw::draw::rect(asw::Quadf(x, y, width, height), asw::Color(200, 200, 220));

  for (size_t i = 0; i < lines.size(); i++) {
    asw::draw::text(font(), lines[i],
                    asw::Vec2f(x + PADDING,
                               y + PADDING - 1 + static_cast<int>(i) * LINE_HEIGHT),
                    i == 0 ? asw::Color(255, 255, 255)
                           : asw::Color(190, 200, 210));
  }

  lines.clear();
}
