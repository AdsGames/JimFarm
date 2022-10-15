/*
  Messenger
  Allan Legemaate
  06/12/2017
  A simple messenger system that stores messages
  and draws them when requested.
*/
#ifndef SRC_MESSENGER_H_
#define SRC_MESSENGER_H_

#include <asw/asw.h>
#include <string>
#include <vector>

class Messenger {
 public:
  Messenger(int listSize = 1,
            bool topDown = false,
            int padding = 5,
            asw::Color font_col = asw::util::makeColor(255, 255, 255),
            asw::Color bg_col = asw::util::makeColor(0, 0, 0, 0));

  unsigned int messageCount();
  void draw(int x, int y);
  void setColors(asw::Color font_col, asw::Color bg_col);

  void pushMessage(std::string new_message);

 private:
  unsigned int max_size;

  bool topDown;

  int padding;

  asw::Color font_col;
  asw::Color bg_col;

  std::vector<std::string> msgs;

  asw::Font pixelart;
};

#endif  // SRC_MESSENGER_H_
