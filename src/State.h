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

#include <asw/asw.h>

#include <memory>
#include <vector>

// Class
class State;

// Game states
enum class ProgramState { NONE, EXIT, MENU, GAME, GAME_MENU };

/*****************
 * STATE ENGINE
 *****************/
class StateEngine {
 public:
  // Init
  StateEngine() = default;

  // Update
  void update();

  // Draw
  void draw();

  // Set next state
  void setNextState(ProgramState state);

  // Get state id
  ProgramState getStateId() const;

 private:
  // Change state
  void changeState();

  // Stores states
  std::unique_ptr<State> state = nullptr;

  // Next state
  ProgramState nextState = ProgramState::NONE;

  // State id
  ProgramState currentState = ProgramState::NONE;
};

/*********
 * STATE
 *********/
class State {
 public:
  // Virtual destructor
  virtual ~State() = default;

  // Draw to screen
  virtual void draw() = 0;

  // Update logic
  virtual void update(StateEngine* engine) = 0;

  // Change state
  void setNextState(StateEngine* engine, ProgramState state) const;
};

#endif  // SRC_STATE_H_
