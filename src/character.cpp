#include "Character.h"

#include <math.h>

#include "KeyListener.h"
#include "MouseListener.h"
#include "Tools.h"

#include "tile_defs.h"
#include "item_defs.h"

#include "Graphics.h"

// Top of head
CharacterForeground::CharacterForeground(Character *charPtr) :
  Sprite(0, 0, 5) {
  char_ptr = charPtr;
  Graphics::Instance() -> add(this);
}

void CharacterForeground::draw(BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2) {
  this -> x = char_ptr -> x;
  this -> y = char_ptr -> y;
  masked_blit (char_ptr -> image, tempBuffer, floor(char_ptr -> ani_ticker/4) * 16, (char_ptr -> direction - 1) * 20, x - char_ptr -> map_pointer -> getX(), y - char_ptr -> map_pointer -> getY() - 8, 16, 8);
}

// Ctor for character
Character::Character() :
  Sprite(0, 0, 2) {
  moving = false;
  direction = 1;
  character_inv = Inventory(10);
  selected_item = 0;

  c_fore = new CharacterForeground(this);
  Graphics::Instance() -> add(this);
}

// World object to point to (needs this!)
void Character::setWorld (World *newTileMap) {
  map_pointer = newTileMap;
}

// Set image
void Character::load_data() {
  // Images
  image = load_bitmap_ex("images/character_1.png");
  inventory_gui = load_bitmap_ex("images/GUI_INVENTORY.png");
  indicator = load_bitmap_ex("images/indicator.png");
  coin = load_bitmap_ex("images/coin.png");

  // Sounds
  pickup = load_sample_ex("sfx/pickup.wav");
  drop = load_sample_ex("sfx/drop.wav");
  step[0] = load_sample_ex("sfx/step_1.wav");
  step[1] = load_sample_ex("sfx/step_2.wav");

  // Load fonts
  pixelart = load_font_ex("fonts/pixelart_condensed.pcx");
  font = pixelart;

  character_inv.addItem(new Item(ITEM_AXE), 0);
  character_inv.addItem(new Item(ITEM_SCYTHE), 1);
  character_inv.addItem(new Item(ITEM_SHOVEL), 2);
  character_inv.addItem(new Item(ITEM_HOE), 3);
  character_inv.addItem(new Item(ITEM_BERRY_SEED), 4);
}

// Draw character to screen
void Character::draw (BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2) {
  // Indicator
  indicator_x = mouse_x * ((map_pointer -> VIEWPORT_WIDTH  / map_pointer -> VIEWPORT_ZOOM) / SCREEN_W) + map_pointer -> getX();
  indicator_y = mouse_y * ((map_pointer -> VIEWPORT_HEIGHT / map_pointer -> VIEWPORT_ZOOM) / SCREEN_H) + map_pointer -> getY();
  indicator_x -= indicator_x % 16;
  indicator_y -= indicator_y % 16;

  // Only item if hands arent empty
  draw_trans_sprite (tempBuffer, indicator, indicator_x - map_pointer -> getX(), indicator_y - map_pointer -> getY());

  // Draw frame
  masked_blit (image, tempBuffer, floor(ani_ticker/4) * 16, (direction - 1) * 20, x - map_pointer -> getX(), y - map_pointer -> getY() - 8, 16, 20);

  // Selected item
  if (character_inv.getItem(selected_item) != NULL) {
    character_inv.getItem(selected_item) -> draw (x - map_pointer -> getX(), y - map_pointer -> getY(), tempBuffer);
  }
}

// Update player
void Character::draw_inventory(BITMAP *tempBuffer) {
  const int draw_x = (tempBuffer -> w - character_inv.getMaxSize() * 18) / 2;
  const int draw_y = tempBuffer -> h - 20;

  // Draw items
  for (int i = 0; i < character_inv.getMaxSize(); i++) {
    draw_sprite (tempBuffer, inventory_gui, 18 * i + draw_x, draw_y);
    if (i == selected_item)
      draw_sprite (tempBuffer, indicator, 18 * i + 1 + draw_x, 1 + draw_y);
    if (character_inv.getItem(i) != NULL) {
      character_inv.getItem(i) -> draw (18 * i + 1 + draw_x, 1 + draw_y, tempBuffer);
      if (i == selected_item)
        textprintf_ex (tempBuffer, pixelart, 1 + draw_x, 21 + draw_y, makecol(255,255,255), -1, character_inv.getItem(i) -> getName().c_str());
    }
  }


  textprintf_ex (tempBuffer, pixelart, 60, 22, makecol(255,255,255), -1, map_pointer -> world_map -> get_biome_at(x, y).c_str());
}

// Update player
void Character::update() {
  // Ask joystick for keys
  poll_joystick();

  // Oh
  // Snap
  if (x % 16 == 0 && y % 16 == 0)
    moving = false;

  // Selector
  if (KeyListener::keyPressed[KEY_Z] || MouseListener::mouse_z_change > 0)
    selected_item = (selected_item + (character_inv.getMaxSize() - 1)) % character_inv.getMaxSize();
  if (KeyListener::keyPressed[KEY_X] || MouseListener::mouse_z_change < 0)
    selected_item = (selected_item + 1) % character_inv.getMaxSize();

  // Movement code
  // Move
  if (!moving) {
    // Up
    if (key[KEY_UP] || key[KEY_W] || joy[0].stick[0].axis[1].d1) {
      direction = DIR_UP;
      if (!map_pointer -> world_map -> solid_at (x, y - 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Down
    else if (key[KEY_DOWN] || key[KEY_S] || joy[0].stick[0].axis[1].d2) {
      direction = DIR_DOWN;
      if (!map_pointer -> world_map -> solid_at (x, y + 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Left
    else if (key[KEY_LEFT] || key[KEY_A] || joy[0].stick[0].axis[0].d1) {
      direction = DIR_LEFT;
      if (!map_pointer -> world_map -> solid_at (x - 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Right
    else if (key[KEY_RIGHT] || key[KEY_D] || joy[0].stick[0].axis[0].d2) {
      direction = DIR_RIGHT;
      if (!map_pointer -> world_map -> solid_at (x + 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }

    // Walk sounds
    if (moving)
      play_sample (step[sound_step], 50, 125, 1300, 0);
  }

  // Update movement
  if (moving) {
    // Smooth move
    if (direction == DIR_UP && y > 0)
      y -= 2;
    else if (direction == DIR_DOWN && y < (map_pointer -> world_map -> getHeight() * 16) - 16)
      y += 2;
    else if (direction == DIR_LEFT && x > 0)
      x -= 2;
    else if (direction == DIR_RIGHT && x < (map_pointer -> world_map -> getWidth() * 16)  - 16)
      x += 2;

    // Increase animation ticker
    ani_ticker = (ani_ticker + 1) % 16;
  }

  // Pickup and drop
  if (KeyListener::keyPressed[KEY_LCONTROL] || KeyListener::keyPressed[KEY_E] || MouseListener::mouse_pressed & 2 || joy[0].button[2].b ) {
    MapItem *itemAtPos  = map_pointer -> world_map -> item_at (roundf (x / 16.0f) * 16, roundf (y / 16.0f) * 16);
    Item *itemInHand = character_inv.getItem(selected_item);

    // Pickup
    if (itemInHand == NULL && itemAtPos != nullptr && itemAtPos -> itemPtr != nullptr) {
      play_sample (pickup, 255, 125, 1000, 0);
      if (character_inv.addItem (itemAtPos -> itemPtr, selected_item)) {
        map_pointer -> world_map -> remove_item (itemAtPos);
        map_pointer -> getMessenger() -> push_message ("You pick up a " + character_inv.getItem(selected_item) -> getName());
      }
    }
    // Drop
    else if (itemInHand != nullptr && itemAtPos == nullptr) {
      play_sample (drop, 255, 125, 1000, 0);
      map_pointer -> world_map -> place_item (character_inv.getItem(selected_item), roundf (x / 16.0f) * 16, roundf (y / 16.0f) * 16);
      if (character_inv.removeItem (selected_item))
        map_pointer -> getMessenger() -> push_message ("You drop your " + itemInHand -> getName());
    }
  }

  // Interact with map
  if (KeyListener::keyPressed[KEY_SPACE] || MouseListener::mouse_pressed & 1 || joy[0].button[0].b) {
    if (character_inv.getItem (selected_item)) {
      map_pointer -> interact (indicator_x, indicator_y, character_inv.getItem (selected_item));
    }
  }

  // Scroll map
  map_pointer -> scroll (x, y);
}
