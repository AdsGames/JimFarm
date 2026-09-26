#ifndef CRAFT_BEHAVIOURS_H_
#define CRAFT_BEHAVIOURS_H_

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

#include "TileBehaviour.h"

// Opens a crafting window (workbench, kiln) at a tier
class StationBehaviour : public TileBehaviour {
 public:
  explicit StationBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  std::string interactVerb(World& world,
                           const std::shared_ptr<Tile>& tile,
                           const ItemStack& held) override;

  int getTier() const { return tier; }

 private:
  std::string window{"crafting"};
  int tier{0};
};

// Rock or ore broken by listed tools, stronger tools take fewer hits
class BreakableBehaviour : public TileBehaviour {
 public:
  explicit BreakableBehaviour(const nlohmann::json& params);

  bool onUse(World& world,
             const std::shared_ptr<Tile>& tile,
             ItemStack& held) override;

  std::string useVerb(World& world,
                      const std::shared_ptr<Tile>& tile,
                      const ItemStack& held) override;

 private:
  std::vector<std::string> tools{};
  int hits{3};
  float energy{3.0F};

  // Item id to min and max count
  std::map<std::string, std::pair<int, int>> drops{};

  // Tile left behind, empty removes it
  std::string into{};
  std::string sound{"axe"};

  // Shown when a tool that is not strong enough is used
  std::string hint{};
};

// Waters tilled soil around it each morning
class SprinklerBehaviour : public TileBehaviour {
 public:
  explicit SprinklerBehaviour(const nlohmann::json& params);

  void onMorning(World& world, const std::shared_ptr<Tile>& tile) override;

  int getRadius() const { return radius; }

 private:
  int radius{1};
};

// Keeps crows off crops around it
class ScarecrowBehaviour : public TileBehaviour {
 public:
  explicit ScarecrowBehaviour(const nlohmann::json& params);

  int getRadius() const { return radius; }

 private:
  int radius{4};
};

// Collects animal products nearby, count kept in meta
class HopperBehaviour : public TileBehaviour {
 public:
  explicit HopperBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  std::string interactVerb(World& world,
                           const std::shared_ptr<Tile>& tile,
                           const ItemStack& held) override;

  // Store one product, false when full or not the collected item
  bool store(const std::shared_ptr<Tile>& tile, const std::string& id) const;

  int getRadius() const { return radius; }

 private:
  std::string item{"item:egg"};
  int radius{4};
};

#endif  // CRAFT_BEHAVIOURS_H_
