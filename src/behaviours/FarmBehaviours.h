#ifndef FARM_BEHAVIOURS_H_
#define FARM_BEHAVIOURS_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "TileBehaviour.h"

// Grows one stage per watered day, harvested by hand when ripe
class CropBehaviour : public TileBehaviour {
 public:
  explicit CropBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) override;

  // Growing info for tooltips
  int getDays() const { return days; }
  int getRegrow() const { return regrow; }
  const std::string& getYield() const { return yield; }
  const std::vector<std::string>& getSeasons() const { return seasons; }
  int getMinTemp() const { return min_temp; }

 private:
  int daysGrown(const std::shared_ptr<Tile>& tile) const;
  bool inSeason(const World& world) const;

  int days{4};
  int regrow{0};
  std::string yield{};
  int yield_min{1};
  int yield_max{1};
  std::string needs_soil{"tile:watered_soil"};
  std::vector<std::string> seasons{};
  int min_temp{-64};
  int max_temp{64};

  // Chance each night crows take the crop when no scarecrow is near
  float crow_chance{0.04F};
};

// Turns into another tile after some days or by chance each day
class TransformBehaviour : public TileBehaviour {
 public:
  explicit TransformBehaviour(const nlohmann::json& params);

  void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) override;

 private:
  std::string into{};
  int days{0};
  float chance{1.0F};
  bool requires_empty_above{false};
  int meta_min{0};
  int meta_max{0};
};

// Animal that wanders, eats and makes a product each fed day
class AnimalBehaviour : public TileBehaviour {
 public:
  explicit AnimalBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  void onTick(World& world, const std::shared_ptr<Tile>& tile) override;

  void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) override;

  bool ticks() const override { return true; }

 private:
  std::string name{"animal"};
  std::string product{};
  std::vector<std::string> food{};
  std::string pickup{};
  float wander{0.01F};
  int leave_days{3};
};

// Wild plant picked by hand, regrows after some days
class ForageBehaviour : public TileBehaviour {
 public:
  explicit ForageBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) override;

 private:
  std::string yield{};
  int yield_min{1};
  int yield_max{1};
  int regrow{3};
};

#endif  // FARM_BEHAVIOURS_H_
