#include "State.h"

#include <iostream>

#include "Game.h"
#include "GameMenu.h"
#include "Menu.h"

/*****************
 * STATE ENGINE
 *****************/

// Draw
void StateEngine::draw() {
  if (state) {
    state->draw();
  }
}

// Update
void StateEngine::update() {
  if (state) {
    state->update(this);
  }
  changeState();
}

// Set next state
void StateEngine::setNextState(ProgramState state) {
  nextState = state;
}

// Get state id
ProgramState StateEngine::getStateId() const {
  return currentState;
}

// Change game screen
void StateEngine::changeState() {
  // If the state needs to be changed
  if (nextState == ProgramState::NONE) {
    return;
  }

  // Delete the current state
  if (state) {
    state.release();
  }

  // Change the state
  switch (nextState) {
    case ProgramState::GAME:
      state = std::make_unique<Game>();
      std::cout << "Switched state to game." << std::endl;
      break;
    case ProgramState::MENU:
      state = std::make_unique<Menu>();
      std::cout << "Switched state to main menu." << std::endl;
      break;
    case ProgramState::OPTIONS:
      // states.push_back(std::make_unique<Options>());
      std::cout << "Switched state to options." << std::endl;
      break;
    case ProgramState::GAME_MENU:
      state = std::make_unique<GameMenu>();
      std::cout << "Switched state to game menu." << std::endl;
      break;
    default:
      std::cout << "Exiting program." << std::endl;
      break;
  }

  // Change the current state ID
  currentState = nextState;

  // NULL the next state ID
  nextState = ProgramState::NONE;
}

/*********
 * STATE
 *********/

// Change state
void State::setNextState(StateEngine* engine, ProgramState state) {
  engine->setNextState(state);
}
