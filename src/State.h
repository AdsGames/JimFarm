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

// Game states
enum class ProgramState { NONE, EXIT, MENU, GAME, GAME_MENU };

#endif  // SRC_STATE_H_
