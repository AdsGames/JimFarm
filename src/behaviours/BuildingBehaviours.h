#ifndef BUILDING_BEHAVIOURS_H_
#define BUILDING_BEHAVIOURS_H_

#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "TileBehaviour.h"

// Opens the store window
class ShopBehaviour : public TileBehaviour {
 public:
  explicit ShopBehaviour(const nlohmann::json& params) { (void)params; }

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;
};

// Sleep to end the day
class BedBehaviour : public TileBehaviour {
 public:
  explicit BedBehaviour(const nlohmann::json& params) { (void)params; }

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;
};

// Burns fuel stored in meta, gives light and warmth while lit
class CampfireBehaviour : public TileBehaviour {
 public:
  explicit CampfireBehaviour(const nlohmann::json& params);

  bool onInteract(World& world,
                  const std::shared_ptr<Tile>& tile,
                  ItemStack& held) override;

  void onTick(World& world, const std::shared_ptr<Tile>& tile) override;

  void onDayEnd(World& world, const std::shared_ptr<Tile>& tile) override;

  bool ticks() const override { return lit; }

 private:
  bool lit{true};
  std::string lit_tile{"tile:campfire"};
  std::string out_tile{"tile:campfire_out"};
  std::map<std::string, int> fuel{};
  int overnight_burn{100};
};

#endif  // BUILDING_BEHAVIOURS_H_
