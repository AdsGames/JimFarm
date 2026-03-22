#include "RecipeManager.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

std::vector<Recipe> RecipeManager::recipes;

int RecipeManager::loadRecipes(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  recipes.clear();

  for (auto const& data : nlohmann::json::parse(file)) {
    Recipe recipe;
    recipe.station = data["station"];
    recipe.inputs = data["inputs"].get<std::map<std::string, int>>();
    recipe.output = data["output"];
    recipe.count = data.value("count", 1);
    recipes.push_back(recipe);
  }

  return 0;
}

const Recipe* RecipeManager::match(
    const std::string& station,
    const std::vector<std::shared_ptr<ItemStack>>& stacks) {
  // Total of each item type in the input slots
  std::map<std::string, int> totals;
  for (auto const& stack : stacks) {
    if (stack && stack->getItem()) {
      totals[stack->getItem()->getType().getId()] += stack->getQuantity();
    }
  }

  if (totals.empty()) {
    return nullptr;
  }

  for (auto const& recipe : recipes) {
    if (recipe.station != station || recipe.inputs.size() != totals.size()) {
      continue;
    }

    const bool enough =
        std::ranges::all_of(recipe.inputs, [&](auto const& input) {
          auto found = totals.find(input.first);
          return found != totals.end() && found->second >= input.second;
        });

    if (enough) {
      return &recipe;
    }
  }

  return nullptr;
}

void RecipeManager::consume(
    const Recipe& recipe,
    const std::vector<std::shared_ptr<ItemStack>>& stacks) {
  for (auto const& [id, quantity] : recipe.inputs) {
    int left = quantity;

    for (auto const& stack : stacks) {
      if (left <= 0) {
        break;
      }

      if (stack && stack->getItem() &&
          stack->getItem()->getType().getId() == id) {
        const int taken = std::min(left, stack->getQuantity());
        stack->remove(taken);
        left -= taken;
      }
    }
  }
}
