/*
  Messenger
  Allan Legemaate
  06/12/2017
  A simple messenger system that stores messages
  and draws them when requested.
*/
#ifndef SRC_MESSENGER_H_
#define SRC_MESSENGER_H_

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

  unsigned int messageCount();
  void draw(BITMAP* buffer, int x, int y);
  void setColors(int font_col, int bg_col);

  void pushMessage(std::string new_message);

 private:
  bool topDown;
  unsigned int max_size;
  int padding;

  int bg_col;
  int font_col;

  std::vector<std::string> msgs;
};

#endif  // SRC_MESSENGER_H_
