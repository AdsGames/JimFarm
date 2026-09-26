#include "UiLabel.h"

#include "../GameState.h"
#include "../manager/RecipeManager.h"

UiLabel::UiLabel(const asw::Vec2i& pos,
                 const std::string& text,
                 bool show_station)
    : UiElement(pos), text(text), show_station(show_station) {}

void UiLabel::draw(const asw::Vec2i& parent_pos, const GameState& state) {
  auto draw_pos = parent_pos + getPosition();

  if (show_station) {
    Recipe station;
    station.station = "crafting";
    station.tier = state.crafting_tier;
    asw::draw::text(font, RecipeManager::stationName(station),
                    asw::Vec2f(draw_pos.x, draw_pos.y),
                    asw::Color(200, 230, 160));
    return;
  }

  asw::draw::text(font, text, asw::Vec2f(draw_pos.x, draw_pos.y),
                  asw::Color(255, 255, 255));
}
