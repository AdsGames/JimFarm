/**
 * State for machine and State Engine
 * Allan Legemaate
 * 30/12/2016
 * Compartmentalize program into states
 *   which can handle only their own logic,
 *   drawing and transitions
 */

#ifndef SRC_STATE_H_
#define SRC_STATE_H_

#include <functional>

// Game states
enum class ProgramState { NONE, MENU, GAME, GAME_MENU };

// What the game scene does on its next init
enum class GameAction { NewGame, Continue, Resume };

inline GameAction pending_game_action = GameAction::NewGame;

// Saves the running game, set by the game scene
inline std::function<bool()> save_current_game = nullptr;

#endif  // SRC_STATE_H_
