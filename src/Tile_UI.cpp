#include "Tile_UI.h"

Tile_UI::Tile_UI() {
  is_open = false;

  inv = new Inventory(64);

  //ui = new UIHandler();
}

Tile_UI::~Tile_UI() {

}


Tile_UI::open() {
  is_open = true;
}


Tile_UI::close() {
  is_open = false;
}

Tile_UI::toggle_ui() {
  is_open = !is_open;
}

Tile_UI::draw() {

}

Tile_UI::update() {

}
