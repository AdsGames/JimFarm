#include "keyListener.h"

keyListener::keyListener()
{
  //ctor
}

keyListener::~keyListener()
{
  //dtor
}


// Check those keys!
void keyListener::update(){
  // Check key just pressed
  for( int i = 0; i < KEY_MAX; i++){
    // Clear old values
    keyPressed[i] = false;
    keyReleased[i] = false;

    // Pressed since last tick?
    if( key[i] == true && lastTicksKey[i]== false){
      keyPressed[i] = true;
    }

    // Released since last tick?
    if( key[i] == false && lastTicksKey[i]== true){
      keyReleased[i] = true;
    }
  }


  // Get new values
  for( int i = 0; i < KEY_MAX; i++){
    lastTicksKey[i] = key[i];
  }
}
