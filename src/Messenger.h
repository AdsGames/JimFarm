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
  Messenger(unsigned int list_size = 1,
            bool is_top_down = false,
            int padding = 5);

  size_t messageCount() const;

  void draw(int x, int y);
  void setColors(asw::Color font, asw::Color background);

  void pushMessage(const std::string& message);

 private:
  unsigned int max_size{1};

  bool top_down{false};

  int padding{5};

  asw::Color font_color{asw::util::makeColor(255, 255, 255)};
  asw::Color bg_color{asw::util::makeColor(0, 0, 0, 0)};

  std::vector<std::string> msgs{};

  asw::Font pixelart{};
};

#endif  // SRC_MESSENGER_H_
