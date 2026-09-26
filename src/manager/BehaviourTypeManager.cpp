#include "BehaviourTypeManager.h"

#include "../behaviours/BuildingBehaviours.h"
#include "../behaviours/CraftBehaviours.h"
#include "../behaviours/FarmBehaviours.h"
#include "../behaviours/TileBehaviour.h"
#include "../behaviours/TreeTileBehaviour.h"

std::map<std::string, BehaviourTypeManager::Factory, std::less<>>
    BehaviourTypeManager::factories;

namespace {
template <typename T>
BehaviourTypeManager::Factory make() {
  return [](const nlohmann::json& params) {
    return std::make_shared<T>(params);
  };
}
}  // namespace

int BehaviourTypeManager::loadBehaviours() {
  factories.clear();
  factories["behaviour:tree"] = make<TreeBehaviour>();
  factories["behaviour:crop"] = make<CropBehaviour>();
  factories["behaviour:transform"] = make<TransformBehaviour>();
  factories["behaviour:animal"] = make<AnimalBehaviour>();
  factories["behaviour:forage"] = make<ForageBehaviour>();
  factories["behaviour:shop"] = make<ShopBehaviour>();
  factories["behaviour:bed"] = make<BedBehaviour>();
  factories["behaviour:campfire"] = make<CampfireBehaviour>();
  factories["behaviour:door"] = make<DoorBehaviour>();
  factories["behaviour:station"] = make<StationBehaviour>();
  factories["behaviour:breakable"] = make<BreakableBehaviour>();
  factories["behaviour:sprinkler"] = make<SprinklerBehaviour>();
  factories["behaviour:scarecrow"] = make<ScarecrowBehaviour>();
  factories["behaviour:hopper"] = make<HopperBehaviour>();

  return 0;
}

std::shared_ptr<TileBehaviour> BehaviourTypeManager::create(
    const std::string& id,
    const nlohmann::json& params) {
  auto found = factories.find(id);
  if (found == factories.end()) {
    throw std::runtime_error("Behaviour not found with id: " + id);
  }

  return found->second(params);
}
