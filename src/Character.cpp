#include "Character.h"

#include <math.h>
#include <iostream>

#include "utility/KeyListener.h"
#include "utility/MouseListener.h"
#include "utility/Tools.h"

#include "manager/interface_defs.h"
#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "manager/InterfaceTypeManager.h"
#include "manager/TileTypeManager.h"

#include "Graphics.h"

// Top of head
CharacterForeground::CharacterForeground(Character* charPtr) : Sprite(0, 0, 5) {
  char_ptr = charPtr;
  Graphics::Instance()->add(this);
}

void CharacterForeground::draw(BITMAP* tempBuffer,
                               float x_1,
                               float y_1,
                               float x_2,
                               float y_2) {
  this->x = char_ptr->x;
  this->y = char_ptr->y;
  masked_blit(char_ptr->image, tempBuffer, floor(char_ptr->ani_ticker / 4) * 16,
              (char_ptr->direction - 1) * 20, x - char_ptr->map_pointer->getX(),
              y - char_ptr->map_pointer->getY() - 8, 16, 8);
}

// Ctor for character
Character::Character() : Sprite(0, 0, 2) {
  moving = false;
  direction = 1;
  selected_item = 0;

  sound_step = false;
  ani_ticker = 0;

  // UI
  inventory_ui = nullptr;
  attatched_ui = nullptr;
  ui_open = false;

  // Character foreground
  c_fore = new CharacterForeground(this);
  Graphics::Instance()->add(this);
}

// World object to point to (needs this!)
void Character::setWorld(World* newTileMap) {
  map_pointer = newTileMap;
}

// Set image
void Character::loadData() {
  // Images
  image = loadBitmap("images/character_1.png");
  inventory_gui = loadBitmap("images/GUI_INVENTORY.png");
  indicator = loadBitmap("images/indicator.png");
  indicator_2 = loadBitmap("images/indicator_2.png");
  coin = loadBitmap("images/coin.png");

  // Sounds
  pickup = loadSample("sfx/pickup.wav");
  drop = loadSample("sfx/drop.wav");
  step[0] = loadSample("sfx/step_1.wav");
  step[1] = loadSample("sfx/step_2.wav");

  // Load fonts
  pixelart = loadFont("fonts/pixelart_condensed.pcx");
  font = pixelart;

  inventory_ui = InterfaceTypeManager::getInterfaceById(0);

  character_inv = inventory_ui->getInventory();

  character_inv->addItem(new Item(ITEM_AXE), 1);
  character_inv->addItem(new Item(ITEM_SCYTHE), 1);
  character_inv->addItem(new Item(ITEM_SHOVEL), 1);
  character_inv->addItem(new Item(ITEM_HOE), 1);
  character_inv->addItem(new Item(ITEM_BERRY_SEED), 4);
}

// Draw character to screen
void Character::draw(BITMAP* tempBuffer,
                     float x_1,
                     float y_1,
                     float x_2,
                     float y_2) {
  // Indicator
  indicator_x =
      mouse_x * ((map_pointer->VIEWPORT_WIDTH / map_pointer->VIEWPORT_ZOOM) /
                 SCREEN_W) +
      map_pointer->getX();
  indicator_y =
      mouse_y * ((map_pointer->VIEWPORT_HEIGHT / map_pointer->VIEWPORT_ZOOM) /
                 SCREEN_H) +
      map_pointer->getY();

  // Cursor
  rectfill(tempBuffer, indicator_x - map_pointer->getX(),
           indicator_y - map_pointer->getY(),
           indicator_x + 2 - map_pointer->getX(),
           indicator_y + 2 - map_pointer->getY(), makecol(255, 255, 255));

  indicator_x -= indicator_x % 16;
  indicator_y -= indicator_y % 16;

  // Only item if hands arent empty
  draw_trans_sprite(tempBuffer, indicator, indicator_x - map_pointer->getX(),
                    indicator_y - map_pointer->getY());

  // Draw frame
  masked_blit(image, tempBuffer, floor(ani_ticker / 4) * 16,
              (direction - 1) * 20, x - map_pointer->getX(),
              y - map_pointer->getY() - 8, 16, 20);

  // Selected item
  if (character_inv->getStack(selected_item)->getItem()) {
    character_inv->getStack(selected_item)
        ->getItem()
        ->draw(x - map_pointer->getX(), y - map_pointer->getY(), tempBuffer);
  }
}

// Update player
void Character::drawInventory(BITMAP* tempBuffer) {
  const int draw_x = (tempBuffer->w - HOTBAR_SIZE * 18) / 2;
  const int draw_y = tempBuffer->h - 20;

  // Draw items
  for (int i = 0; i < HOTBAR_SIZE; i++) {
    draw_sprite(tempBuffer, inventory_gui, 18 * i + draw_x, draw_y);
    if (i == selected_item)
      draw_sprite(tempBuffer, indicator, 18 * i + 1 + draw_x, 1 + draw_y);
    if (character_inv->getStack(i)) {
      character_inv->getStack(i)->draw(18 * i + 1 + draw_x, 1 + draw_y,
                                       tempBuffer);
      if (i == selected_item && character_inv->getStack(i)->getItem())
        textprintf_ex(tempBuffer, pixelart, 1 + draw_x, 21 + draw_y,
                      makecol(255, 255, 255), -1,
                      character_inv->getStack(i)->getItem()->getName().c_str());
    }
  }

  // Only item if hands arent empty
  draw_trans_sprite(tempBuffer, indicator_2, indicator_x - map_pointer->getX(),
                    indicator_y - map_pointer->getY());

  // Draw UI
  if (ui_open && attatched_ui) {
    attatched_ui->draw(tempBuffer);
  }
}

// Update player
void Character::update() {
  // Ask joystick for keys
  poll_joystick();

  // Open UI
  if (KeyListener::keyPressed[KEY_E]) {
    if (ui_open) {
      ui_open = false;
      attatched_ui = nullptr;
    } else {
      ui_open = true;
      attatched_ui = inventory_ui;
    }
  }

  if (KeyListener::keyPressed[KEY_G]) {
    if (ui_open) {
      ui_open = false;
      attatched_ui = nullptr;
    } else {
      ui_open = true;
      attatched_ui = InterfaceTypeManager::getInterfaceById(1);
    }
  }

  // UI Open
  if (ui_open && attatched_ui != nullptr) {
    // Update UI
    attatched_ui->update(map_pointer);
    return;
  }

  // Oh
  // Snap
  if (x % 16 == 0 && y % 16 == 0)
    moving = false;

  // Selector
  if (KeyListener::keyPressed[KEY_Z] || MouseListener::mouse_z_change > 0)
    selected_item = (selected_item + (HOTBAR_SIZE - 1)) % HOTBAR_SIZE;
  if (KeyListener::keyPressed[KEY_X] || MouseListener::mouse_z_change < 0)
    selected_item = (selected_item + 1) % HOTBAR_SIZE;

  // Movement code
  // Move
  if (!moving) {
    // Up
    if (key[KEY_UP] || key[KEY_W] || joy[0].stick[0].axis[1].d1) {
      direction = DIR_UP;
      if (!map_pointer->world_map->isSolidAt(x, y - 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Down
    else if (key[KEY_DOWN] || key[KEY_S] || joy[0].stick[0].axis[1].d2) {
      direction = DIR_DOWN;
      if (!map_pointer->world_map->isSolidAt(x, y + 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Left
    else if (key[KEY_LEFT] || key[KEY_A] || joy[0].stick[0].axis[0].d1) {
      direction = DIR_LEFT;
      if (!map_pointer->world_map->isSolidAt(x - 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Right
    else if (key[KEY_RIGHT] || key[KEY_D] || joy[0].stick[0].axis[0].d2) {
      direction = DIR_RIGHT;
      if (!map_pointer->world_map->isSolidAt(x + 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }

    // Walk sounds
    if (moving)
      play_sample(step[sound_step], 50, 125, 1300, 0);
  }

  // Update movement
  if (moving) {
    // Smooth move
    if (direction == DIR_UP && y > 0)
      y -= 2;
    else if (direction == DIR_DOWN &&
             y < (map_pointer->world_map->getHeight() * 16) - 16)
      y += 2;
    else if (direction == DIR_LEFT && x > 0)
      x -= 2;
    else if (direction == DIR_RIGHT &&
             x < (map_pointer->world_map->getWidth() * 16) - 16)
      x += 2;

    // Increase animation ticker
    ani_ticker = (ani_ticker + 1) % 16;
  } else {
    ani_ticker = 0;
  }

  // Pickup
  MapItem* itemAtPos = map_pointer->world_map->getItemAt(
      roundf(x / 16.0f) * 16, roundf(y / 16.0f) * 16);

  // Pickup
  if (itemAtPos != nullptr) {
    play_sample(pickup, 255, 125, 1000, 0);
    if (character_inv->addItem(itemAtPos->itemPtr, 1)) {
      map_pointer->world_map->removeItem(itemAtPos);
    }
  }

  // Drop
  if (KeyListener::keyPressed[KEY_F] || MouseListener::mouse_pressed & 2 ||
      joy[0].button[2].b) {
    Item* itemInHand = nullptr;
    if (character_inv->getStack(selected_item)->getItem()) {
      itemInHand = character_inv->getStack(selected_item)->getItem();
    }

    // Drop
    if (itemInHand != nullptr) {
      play_sample(drop, 255, 125, 1000, 0);
      map_pointer->world_map->placeItemAt(itemInHand, indicator_x, indicator_y);
      character_inv->getStack(selected_item)->remove(1);
    }
  }

  // Interact with map
  if (KeyListener::keyPressed[KEY_SPACE] || MouseListener::mouse_pressed & 1 ||
      joy[0].button[0].b) {
    if (character_inv->getStack(selected_item)->getItem()) {
      map_pointer->interact(indicator_x, indicator_y,
                            character_inv->getStack(selected_item)->getItem());
    }
  }

  // Scroll map
  map_pointer->scroll(x, y);
}
