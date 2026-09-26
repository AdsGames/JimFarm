#include "TreeTileBehaviour.h"

#include <algorithm>

#include "../World.h"
#include "../manager/ItemTypeManager.h"
#include "../manager/SoundManager.h"
#include "../utility/Tools.h"

TreeBehaviour::TreeBehaviour(const nlohmann::json& params)
    : tools(params.value("tools", std::vector<std::string>{"item:axe"})),
      hits(std::max(1, params.value("hits", 3))),
      energy(params.value("energy", 4.0F)),
      sapling_chance(params.value("sapling_chance", 0.3F)) {}

bool TreeBehaviour::onInteract(World& world,
                               const std::shared_ptr<Tile>& tile,
                               ItemStack& held) {
  const auto item = held.getItem();
  if (!item || std::ranges::find(tools, item->getType().getId()) ==
                   tools.end()) {
    return false;
  }

  if (!world.getState().useEnergy(energy)) {
    SoundManager::play("error");
    return true;
  }

  SoundManager::play("axe");
  world.burstParticles("wood", tile->getTilePosition());

  // Each hit takes a share of the tree's hitpoints, better axes take more
  const float power =
      std::max(1.0F, ItemTypeManager::getInfo(item->getType().getId()).power);
  tile->damage(static_cast<unsigned char>(
      std::min(255.0F, (255.0F / hits + 1.0F) * power)));
  if (tile->getHitpoints() > 0) {
    return true;
  }

  // Cut down tree
  const auto tile_pos = tile->getTilePosition();
  world.getMap().replaceTile(tile, "tile:stump");
  world.burstParticles("leaves", tile_pos);
  world.dropItems("item:stick", tile_pos, 2);
  world.dropItems("item:wood", tile_pos, 2);

  if (random(0, 99) < static_cast<int>(sapling_chance * 100.0F)) {
    world.dropItems("item:sapling", tile_pos);
  }

  return true;
}
