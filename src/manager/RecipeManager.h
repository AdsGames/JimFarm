#ifndef RECIPE_MANAGER_H_
#define RECIPE_MANAGER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../ItemStack.h"

// Crafting tiers, better stations make more recipes
constexpr int TIER_HAND = 0;
constexpr int TIER_WORKBENCH = 1;
constexpr int TIER_STONE_WORKBENCH = 2;

// Shapeless recipe, the input slots must hold exactly these item types
struct Recipe {
  std::string station{};
  std::map<std::string, int> inputs{};
  std::string output{};
  int count{1};

  // Lowest station tier that can make it
  int tier{TIER_HAND};
};

class RecipeManager {
 public:
  static int loadRecipes(const std::string& path);

  // First recipe for station the stacks can make at tier, or nullptr
  static const Recipe* match(
      const std::string& station,
      const std::vector<std::shared_ptr<ItemStack>>& stacks,
      int tier = TIER_HAND);

  // Recipe the stacks would make at a higher tier, or nullptr
  static const Recipe* needsTier(
      const std::string& station,
      const std::vector<std::shared_ptr<ItemStack>>& stacks,
      int tier);

  // Station name shown to the player, e.g. "Workbench"
  static std::string stationName(const Recipe& recipe);

  // Remove one craft worth of inputs from stacks
  static void consume(const Recipe& recipe,
                      const std::vector<std::shared_ptr<ItemStack>>& stacks);

  static const std::vector<Recipe>& getRecipes() { return recipes; }

 private:
  static std::vector<Recipe> recipes;
};

#endif  // RECIPE_MANAGER_H_
