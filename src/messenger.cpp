#include "messenger.h"

messenger::messenger( int listSize, bool topDown, int padding, int font_col, int bg_col){
  this -> max_size = listSize;
  this -> topDown = topDown;
  this -> padding = padding;
  this -> font_col = font_col;
  this -> bg_col = bg_col;
}

messenger::~messenger(){}

void messenger::setColors( int font_col, int bg_col){
  this -> font_col = font_col;
  this -> bg_col = bg_col;
}

void messenger::push_message( std::string new_message){
  msgs.push_back( new_message);
  if( numberMessages() > this -> max_size)
    msgs.erase( msgs.begin() );
}

unsigned int messenger::numberMessages(){
  return msgs.size();
}

void messenger::draw( BITMAP *buffer, int x, int y){
  int offset = 0;
  for( unsigned int i = 0; i < numberMessages(); i++){
    textprintf_ex( buffer, font, x, y + offset, font_col, bg_col, ">%s %d", msgs.at(i).c_str(), numberMessages());

    if( topDown)
      offset += text_height( font) + padding;
    else
      offset -= text_height( font) + padding;
  }
}
