#ifndef TREE_BEHAVIOUR_H_
#define TREE_BEHAVIOUR_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "TileBehaviour.h"

// Chopped down over several hits, leaves a stump and drops wood
class TreeBehaviour : public TileBehaviour {
 public:
  explicit TreeBehaviour(const nlohmann::json& params);

  bool onUse(World& world,
             const std::shared_ptr<Tile>& tile,
             ItemStack& held) override;

  std::string useVerb(World& world,
                      const std::shared_ptr<Tile>& tile,
                      const ItemStack& held) override;

 private:
  std::vector<std::string> tools{};
  int hits{3};
  float energy{4.0F};
  float sapling_chance{0.3F};
};

#endif  // TREE_BEHAVIOUR_H_
