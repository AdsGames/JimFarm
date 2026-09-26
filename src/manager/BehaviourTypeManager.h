#ifndef BEHAVIOUR_TYPE_MANAGER_H_
#define BEHAVIOUR_TYPE_MANAGER_H_

#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "../behaviours/TileBehaviour.h"

class BehaviourTypeManager {
 public:
  using Factory =
      std::function<std::shared_ptr<TileBehaviour>(const nlohmann::json&)>;

  // Register behaviour types
  static int loadBehaviours();

  // Create a behaviour instance with params from tiles.json
  static std::shared_ptr<TileBehaviour> create(const std::string& id,
                                               const nlohmann::json& params);

 private:
  static std::map<std::string, Factory, std::less<>> factories;
};

#endif  // BEHAVIOUR_TYPE_MANAGER_H_
