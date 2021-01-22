/*
  Messenger
  Allan Legemaate
  06/12/2017
  A simple messenger system that stores messages
  and draws them when requested.
*/
#ifndef MESSENGER_H
#define MESSENGER_H

#include <allegro.h>
#include <string>
#include <vector>

class Messenger {
 public:
  Messenger(int listSize = 1,
            bool topDown = false,
            int padding = 5,
            int font_col = 0xFFFFFF,
            int bg_col = -1);
  virtual ~Messenger();

  unsigned int numberMessages();
  void draw(BITMAP* buffer, int x, int y);
  void setColors(int font_col, int bg_col);

  void push_message(std::string new_message);

 protected:
 private:
  bool topDown;
  unsigned int max_size;
  int padding;

  int bg_col;
  int font_col;

  std::vector<std::string> msgs;
};

#endif  // MESSENGER_H
