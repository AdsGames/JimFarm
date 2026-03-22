#ifndef RECIPE_MANAGER_H_
#define RECIPE_MANAGER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../ItemStack.h"

// Shapeless recipe, the input slots must hold exactly these item types
struct Recipe {
  std::string station{};
  std::map<std::string, int> inputs{};
  std::string output{};
  int count{1};
};

class RecipeManager {
 public:
  static int loadRecipes(const std::string& path);

  // First recipe for station the stacks can make, or nullptr
  static const Recipe* match(
      const std::string& station,
      const std::vector<std::shared_ptr<ItemStack>>& stacks);

  // Remove one craft worth of inputs from stacks
  static void consume(const Recipe& recipe,
                      const std::vector<std::shared_ptr<ItemStack>>& stacks);

  static const std::vector<Recipe>& getRecipes() { return recipes; }

 private:
  static std::vector<Recipe> recipes;
};

#endif  // RECIPE_MANAGER_H_
