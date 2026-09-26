#include "Tooltip.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <sstream>
#include <vector>

#include "../Item.h"
#include "../behaviours/FarmBehaviours.h"
#include "../manager/ItemTypeManager.h"
#include "../manager/TileTypeManager.h"
#include "UiScale.h"

namespace {
constexpr int PADDING = 3;
constexpr int LINE_HEIGHT = 9;
constexpr int MAX_TEXT_WIDTH = 140;

const asw::Color TITLE_COLOR(255, 255, 255);
const asw::Color INFO_COLOR(190, 200, 210);
const asw::Color DESCRIPTION_COLOR(170, 220, 160);
const asw::Color SEASON_COLOR(240, 210, 120);
const asw::Color WARNING_COLOR(255, 120, 110);

struct Line {
  std::string text;
  asw::Color color;
};

std::vector<Line> lines{};
Season current_season{Season::Spring};

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

// Adds text split into lines no wider than MAX_TEXT_WIDTH
void addWrapped(const std::string& text, asw::Color color) {
  std::istringstream words(text);
  std::string word;
  std::string line;

  while (words >> word) {
    const auto candidate = line.empty() ? word : line + " " + word;
    if (!line.empty() &&
        asw::util::get_text_size(font(), candidate).x > MAX_TEXT_WIDTH) {
      lines.push_back({line, color});
      line = word;
    } else {
      line = candidate;
    }
  }

  if (!line.empty()) {
    lines.push_back({line, color});
  }
}

std::string joinSeasons(const std::vector<std::string>& seasons) {
  std::string joined;
  for (auto const& season : seasons) {
    joined += (joined.empty() ? "" : ", ") + season;
  }
  return joined.empty() ? "any season" : joined;
}

const CropBehaviour* cropOf(const TileType& tile) {
  for (auto const& behaviour : tile.getBehaviours()) {
    if (auto crop = dynamic_cast<const CropBehaviour*>(behaviour.get())) {
      return crop;
    }
  }
  return nullptr;
}

// Crop the item plants (seeds), or nullptr
const CropBehaviour* cropPlantedBy(const ItemInfo& info) {
  for (auto const& action : info.actions) {
    if (!action.place.empty() && TileTypeManager::exists(action.place)) {
      if (auto crop = cropOf(TileTypeManager::getTile(action.place))) {
        return crop;
      }
    }
  }
  return nullptr;
}

// Crop that yields the item (produce), or nullptr
const CropBehaviour* cropYielding(const std::string& id) {
  for (auto const& [_, tile] : TileTypeManager::getTiles()) {
    auto crop = cropOf(tile);
    if (crop && crop->getYield() == id) {
      return crop;
    }
  }
  return nullptr;
}

void addCropInfo(const std::string& id, const ItemInfo& info) {
  if (auto seed = cropPlantedBy(info)) {
    lines.push_back(
        {"Grows in " + joinSeasons(seed->getSeasons()), SEASON_COLOR});

    std::string timing = std::format("Ready in {} days", seed->getDays());
    if (seed->getRegrow() > 0) {
      timing += std::format(", regrows in {}", seed->getRegrow());
    }
    lines.push_back({timing, SEASON_COLOR});

    if (seed->getMinTemp() > 0) {
      lines.push_back({"Needs warm land", SEASON_COLOR});
    }

    const auto& seasons = seed->getSeasons();
    const auto now = GameState::seasonName(current_season);
    if (!seasons.empty() &&
        std::ranges::find(seasons, now) == seasons.end()) {
      lines.push_back({"Out of season, will not grow", WARNING_COLOR});
    }
    return;
  }

  if (auto crop = cropYielding(id)) {
    lines.push_back(
        {"In season: " + joinSeasons(crop->getSeasons()), SEASON_COLOR});
  }
}
}  // namespace

void Tooltip::setSeason(Season season) {
  current_season = season;
}

void Tooltip::setItem(const Item& item, int price) {
  const auto& id = item.getType().getId();
  const auto& info = ItemTypeManager::getInfo(id);

  lines = {{capitalise(item.getType().getName()), TITLE_COLOR}};

  if (!info.description.empty()) {
    addWrapped(info.description, DESCRIPTION_COLOR);
  }

  addCropInfo(id, info);

  if (price > 0) {
    lines.push_back({std::format("Costs ${}", price), INFO_COLOR});
  } else if (info.value > 0) {
    lines.push_back({std::format("Sells for ${}", info.value), INFO_COLOR});
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
      lines.push_back({food, INFO_COLOR});
    }
  }

  // Containers such as the watering can keep their charge in meta
  if (info.start_meta > 0) {
    lines.push_back(
        {std::format("{}/{} left", item.getMeta(), info.start_meta),
         INFO_COLOR});
  }

  if (info.attack > 0) {
    lines.push_back({std::format("Attack {}", info.attack), INFO_COLOR});
  }

  if (info.carry_warmth > 0) {
    lines.push_back(
        {std::format("Warm +{:.0f}C while carried", info.carry_warmth),
         INFO_COLOR});
  }
}

void Tooltip::draw(const asw::Vec2i& ui_size) {
  if (lines.empty()) {
    return;
  }

  int width = 0;
  for (auto const& line : lines) {
    width = std::max(width, asw::util::get_text_size(font(), line.text).x);
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
    asw::draw::text(
        font(), lines[i].text,
        asw::Vec2f(x + PADDING,
                   y + PADDING - 1 + static_cast<int>(i) * LINE_HEIGHT),
        lines[i].color);
  }

  lines.clear();
}
