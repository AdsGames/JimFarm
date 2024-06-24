#ifndef BEHAVIOUR_TYPE_MANAGER_H_
#define BEHAVIOUR_TYPE_MANAGER_H_

#include <map>
#include <memory>

#include "../behaviours/TileBehaviour.h"

class BehaviourTypeManager {
 public:
  // Load behaviour types
  static int loadBehaviours();

  // Allows communication
  static std::shared_ptr<TileBehaviour> getBehaviour(const std::string& id);

 private:
  static std::map<std::string, std::shared_ptr<TileBehaviour>, std::less<>>
      behaviour_defs;
};

#endif  // BEHAVIOUR_TYPE_MANAGER_H_