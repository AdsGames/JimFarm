#include "State.h"

#include <iostream>

#include "Game.h"
#include "Menu.h"
#include "GameMenu.h"

/*****************
 * STATE ENGINE
 *****************/

// Draw
void StateEngine::draw(BITMAP* buffer) {
  for (unsigned int i = 0; i < states.size(); i++) {
    states.at(i) -> draw(buffer);
  }
}

// Update
void StateEngine::update() {
  if (states.size() > 0) {
    states.at(states.size() - 1) -> update(this);
  }
  changeState();
}

// Set next state
void StateEngine::setNextState(int newState, bool deletePrev) {
  nextState = newState;
  delete_previous = deletePrev;
}

// Get state id
int StateEngine::getStateId() {
  return currentState;
}

// Change game screen
void StateEngine::changeState() {
  // If the state needs to be changed
  if (nextState == STATE_NULL) {
    return;
  }

  // Delete the current state
  if (states.size() > 0 && delete_previous) {
    states.pop_back();
  }

  // Change the state
  switch(nextState) {
    case STATE_LAST:
      std::cout << ("Switched back.\n");
      break;
    case STATE_GAME:
      states.push_back(new Game());
      std::cout << ("Switched state to game.\n");
      break;
    case STATE_MENU:
      states.push_back(new Menu());
      std::cout << ("Switched state to main menu.\n");
      break;
    case STATE_OPTIONS:
      //states.push_back(new Options());
      std::cout << ("Switched state to options.\n");
      break;
    case STATE_GAME_MENU:
      states.push_back(new GameMenu());
      std::cout << ("Switched state to game menu.\n");
      break;
    default:
      states.clear();
      std::cout << ("Exiting program.");
  }

  // Change the current state ID
  currentState = nextState;

  // NULL the next state ID
  nextState = STATE_NULL;
}


/*********
 * STATE
 *********/

// Change state
void State::setNextState(StateEngine* engine, int state, bool delete_state) {
  engine -> setNextState(state, delete_state);
}
