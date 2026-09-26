#include "GameState.h"

#include <algorithm>
#include <format>
#include <utility>

#include "utility/Tools.h"

bool GameState::advanceClock(float dt) {
  minutes += dt * GAME_MINUTES_PER_SECOND;
  return minutes >= DAY_END_MINUTES;
}

void GameState::nextDay() {
  minutes = DAY_START_MINUTES;
  earned_today = 0;
  day++;

  if (day > DAYS_PER_SEASON) {
    day = 1;
    const int next = static_cast<int>(season) + 1;
    if (next >= SEASONS_PER_YEAR) {
      year++;
    }
    season = static_cast<Season>(next % SEASONS_PER_YEAR);
  }

  // Weather, no rain in winter, snow instead
  const int roll = random(0, 99);
  if (season == Season::Winter) {
    weather = roll < 35 ? Weather::Snow : Weather::Sunny;
  } else {
    weather = roll < 20 ? Weather::Rain : Weather::Sunny;
  }
}

int GameState::absoluteDay() const {
  return ((year - 1) * SEASONS_PER_YEAR + static_cast<int>(season)) *
             DAYS_PER_SEASON +
         day;
}

float GameState::darkness() const {
  if (minutes <= DUSK_MINUTES) {
    return 0.0F;
  }

  if (minutes >= NIGHT_MINUTES) {
    return 1.0F;
  }

  return (minutes - DUSK_MINUTES) / (NIGHT_MINUTES - DUSK_MINUTES);
}

bool GameState::isNight() const {
  return minutes >= 20 * 60;
}

std::string GameState::timeString() const {
  const int total = static_cast<int>(minutes);
  const int hour = (total / 60) % 24;
  const int minute = (total % 60) / 10 * 10;
  const int display_hour = hour % 12 == 0 ? 12 : hour % 12;
  return std::format("{}:{:02} {}", display_hour, minute,
                     hour < 12 ? "am" : "pm");
}

std::string GameState::dateString() const {
  return std::format("{} {}, Y{}", seasonName(season), day, year);
}

std::string GameState::seasonName(Season season) {
  switch (season) {
    case Season::Spring:
      return "Spring";
    case Season::Summer:
      return "Summer";
    case Season::Fall:
      return "Fall";
    case Season::Winter:
      return "Winter";
  }
  return "";
}

std::string GameState::weatherName() const {
  switch (weather) {
    case Weather::Sunny:
      return darkness() >= 0.5F ? "Clear" : "Sunny";
    case Weather::Rain:
      return "Rain";
    case Weather::Snow:
      return "Snow";
  }
  return "";
}

bool GameState::spend(int amount) {
  if (amount > coins) {
    return false;
  }

  coins -= amount;
  return true;
}

void GameState::earn(int amount) {
  coins += amount;
  total_earned += amount;
  earned_today += amount;
}

int GameState::lose(int amount) {
  const int lost = std::min(amount, coins);
  coins -= lost;
  return lost;
}

bool GameState::useEnergy(float amount) {
  if (amount <= 0.0F) {
    return true;
  }

  if (energy < amount) {
    notify("Too tired, go to bed");
    return false;
  }

  energy -= amount;
  return true;
}

void GameState::notify(const std::string& message) {
  messages.push_back(message);
}

std::vector<std::string> GameState::takeMessages() {
  return std::exchange(messages, {});
}

nlohmann::json GameState::toJson() const {
  return {
      {"minutes", minutes},
      {"day", day},
      {"year", year},
      {"season", static_cast<int>(season)},
      {"weather", static_cast<int>(weather)},
      {"coins", coins},
      {"total_earned", total_earned},
      {"earned_today", earned_today},
      {"energy", energy},
      {"health", health},
      {"hunger", hunger},
      {"thirst", thirst},
      {"warmth", warmth},
      {"goal_checked", goal_checked},
      {"goal_met", goal_met},
      {"home", {home.x, home.y}},
      {"crops_harvested", crops_harvested},
      {"items_sold", items_sold},
      {"wolves_defeated", wolves_defeated},
      {"known_items", known_items},
      {"order",
       {{"item_id", order.item_id},
        {"quantity", order.quantity},
        {"delivered", order.delivered},
        {"reward", order.reward},
        {"deadline", order.deadline},
        {"active", order.active}}},
  };
}

void GameState::fromJson(const nlohmann::json& data) {
  minutes = data.value("minutes", static_cast<float>(DAY_START_MINUTES));
  day = data.value("day", 1);
  year = data.value("year", 1);
  season = static_cast<Season>(data.value("season", 0));
  weather = static_cast<Weather>(data.value("weather", 0));
  coins = data.value("coins", 0);
  total_earned = data.value("total_earned", 0);
  earned_today = data.value("earned_today", 0);
  energy = data.value("energy", MAX_STAT);
  health = data.value("health", MAX_STAT);
  hunger = data.value("hunger", MAX_STAT);
  thirst = data.value("thirst", MAX_STAT);
  warmth = data.value("warmth", MAX_STAT);
  goal_checked = data.value("goal_checked", false);
  goal_met = data.value("goal_met", false);
  crops_harvested = data.value("crops_harvested", 0);
  items_sold = data.value("items_sold", 0);
  wolves_defeated = data.value("wolves_defeated", 0);
  known_items = data.value("known_items", std::set<std::string>{});

  if (data.contains("home")) {
    home = asw::Vec2i(data["home"][0], data["home"][1]);
  }

  if (data.contains("order")) {
    const auto& o = data["order"];
    order.item_id = o.value("item_id", "");
    order.quantity = o.value("quantity", 0);
    order.delivered = o.value("delivered", 0);
    order.reward = o.value("reward", 0);
    order.deadline = o.value("deadline", 0);
    order.active = o.value("active", false);
  }
}
