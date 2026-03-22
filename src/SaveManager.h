#ifndef SRC_SAVEMANAGER_H_
#define SRC_SAVEMANAGER_H_

#include <string>

class World;
class Character;

// Saves the world, player and inventories to one JSON file
namespace SaveManager {
// Full path of the save file
std::string path();

bool exists();

bool save(const World& world, const Character& player);

// Returns false (and leaves the game untouched) if there is no valid save
bool load(World& world, Character& player);
}  // namespace SaveManager

#endif  // SRC_SAVEMANAGER_H_
