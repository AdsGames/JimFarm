#ifndef SRC_GAMESTATE_H_
#define SRC_GAMESTATE_H_

#include <asw/asw.h>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <vector>

// Clock, in game minutes since midnight
constexpr int DAY_START_MINUTES = 6 * 60;
constexpr int DAY_END_MINUTES = 26 * 60;  // 2am, pass out
constexpr int DUSK_MINUTES = 18 * 60;
constexpr int NIGHT_MINUTES = 21 * 60;
constexpr float GAME_MINUTES_PER_SECOND = 4.0F;

// Calendar
constexpr int DAYS_PER_SEASON = 14;
constexpr int SEASONS_PER_YEAR = 4;

// Goal, total coins earned by the end of the first season
constexpr int GOAL_COINS = 1000;

// Player stats
constexpr float MAX_STAT = 100.0F;

enum class Season { Spring = 0, Summer = 1, Fall = 2, Winter = 3 };

enum class Weather { Sunny = 0, Rain = 1, Snow = 2 };

// A delivery request posted at the store
struct Order {
  std::string item_id{};
  int quantity{0};
  int delivered{0};
  int reward{0};
  int deadline{0};  // Absolute day number
  bool active{false};
};

class GameState {
 public:
  // Clock, returns true when the day runs out (player passes out)
  bool advanceClock(float dt);

  // Moves to the next morning and rolls weather
  void nextDay();

  // Absolute day number, starting at 1
  int absoluteDay() const;

  float getMinutes() const { return minutes; }
  void setMinutes(float value) { minutes = value; }
  int getDay() const { return day; }
  int getYear() const { return year; }
  Season getSeason() const { return season; }
  Weather getWeather() const { return weather; }

  // 0 in daylight, 1 in full night
  float darkness() const;
  bool isNight() const;

  std::string timeString() const;
  std::string dateString() const;
  static std::string seasonName(Season season);
  std::string weatherName() const;

  // Economy
  int getCoins() const { return coins; }
  bool spend(int amount);
  void earn(int amount);

  // Give coins back without counting them as earned
  void refund(int amount) { coins += amount; }

  // Lose coins (e.g. fainting), returns amount lost
  int lose(int amount);
  int getTotalEarned() const { return total_earned; }
  int getEarnedToday() const { return earned_today; }

  // Stats
  float energy{MAX_STAT};
  float health{MAX_STAT};
  float hunger{MAX_STAT};
  float thirst{MAX_STAT};
  float warmth{MAX_STAT};

  // Uses energy if there is enough, else notifies and returns false
  bool useEnergy(float amount);

  // Messages for the player, drained by the world each frame
  void notify(const std::string& message);
  std::vector<std::string> takeMessages();

  // Store order
  Order order{};

  // Goal
  bool goal_checked{false};
  bool goal_met{false};

  // Home (bed) tile
  asw::Vec2i home{0, 0};

  // Set by a bed, handled by the world
  bool sleep_requested{false};

  // Tier of the crafting window, set by the station that opened it
  int crafting_tier{0};

  // Items the player has held, recipes using them are shown
  std::set<std::string> known_items{};

  // Lifetime stats for summaries
  int crops_harvested{0};
  int items_sold{0};
  int wolves_defeated{0};

  // Serialize
  nlohmann::json toJson() const;
  void fromJson(const nlohmann::json& data);

 private:
  float minutes{static_cast<float>(DAY_START_MINUTES)};
  int day{1};
  int year{1};
  Season season{Season::Spring};
  Weather weather{Weather::Sunny};

  int coins{100};
  int total_earned{0};
  int earned_today{0};

  std::vector<std::string> messages{};
};

#endif  // SRC_GAMESTATE_H_
