#include "BehaviourTypeManager.h"

#include <fstream>

#include "../behaviours/TileBehaviour.h"
#include "../behaviours/TreeTileBehaviour.h"

std::map<std::string, std::shared_ptr<TileBehaviour>, std::less<>>
    BehaviourTypeManager::behaviour_defs;

int BehaviourTypeManager::loadBehaviours() {
  behaviour_defs.clear();
  behaviour_defs.insert(
      std::make_pair("behaviour:tree", std::make_shared<TreeBehaviour>()));

  return 0;
}

std::shared_ptr<TileBehaviour> BehaviourTypeManager::getBehaviour(
    const std::string& id) {
  if (!behaviour_defs.contains(id)) {
    return nullptr;
  }

  return behaviour_defs[id];
}