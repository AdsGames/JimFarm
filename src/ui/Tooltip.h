#ifndef SRC_UI_TOOLTIP_H_
#define SRC_UI_TOOLTIP_H_

#include <asw/asw.h>

#include <string>

#include "../GameState.h"

class Item;

// One tooltip per frame, set while drawing slots and drawn last so it sits on
// top of every window
namespace Tooltip {
// Current season, used to warn about out of season seeds
void setSeason(Season season);

// Show an item's name and details at the mouse this frame
void setItem(const Item& item, int price = 0);

// Draw the tooltip set this frame, then clear it
void draw(const asw::Vec2i& ui_size);
}  // namespace Tooltip

#endif  // SRC_UI_TOOLTIP_H_
