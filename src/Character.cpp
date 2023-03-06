#include "Character.h"

#include <math.h>

#include "utility/Tools.h"

#include "manager/interface_defs.h"
#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "manager/InterfaceTypeManager.h"
#include "manager/TileTypeManager.h"

#include "Graphics.h"

// Top of head
CharacterForeground::CharacterForeground(Character* charPtr)
    : Sprite(0, 0, 5), char_ptr(charPtr) {}

void CharacterForeground::draw(float x_1, float y_1, float x_2, float y_2) {
  this->x = char_ptr->x;
  this->y = char_ptr->y;
  asw::draw::stretchSpriteBlit(
      char_ptr->image, floor(char_ptr->ani_ticker / 4) * 16,
      (char_ptr->direction - 1) * 20, 16, 8, x - char_ptr->map_pointer->getX(),
      y - char_ptr->map_pointer->getY() - 8, 16, 8);
}

// Ctor for character
Character::Character() : Sprite(0, 0, 2) {
  c_fore = std::make_shared<CharacterForeground>(this);
  Graphics::Instance()->add(c_fore);
}

// World object to point to (needs this!)
void Character::setWorld(World* map_pointer) {
  this->map_pointer = map_pointer;
}

// Set image
void Character::loadData() {
  // Images
  image = asw::assets::loadTexture("assets/images/character_1.png");
  inventory_gui = asw::assets::loadTexture("assets/images/GUI_INVENTORY.png");
  indicator = asw::assets::loadTexture("assets/images/indicator.png");
  coin = asw::assets::loadTexture("assets/images/coin.png");

  // Sounds
  pickup = asw::assets::loadSample("assets/sfx/pickup.wav");
  drop = asw::assets::loadSample("assets/sfx/drop.wav");
  step[0] = asw::assets::loadSample("assets/sfx/step_1.wav");
  step[1] = asw::assets::loadSample("assets/sfx/step_2.wav");

  // Load fonts
  pixelart = asw::assets::loadFont("assets/fonts/pixelart.ttf", 12);

  inventory_ui = InterfaceTypeManager::getInterfaceById(0);

  character_inv = inventory_ui->getInventory();

  character_inv->addItem(std::make_shared<Item>(ITEM_AXE), 1);
  character_inv->addItem(std::make_shared<Item>(ITEM_SCYTHE), 1);
  character_inv->addItem(std::make_shared<Item>(ITEM_SHOVEL), 1);
  character_inv->addItem(std::make_shared<Item>(ITEM_HOE), 1);
  character_inv->addItem(std::make_shared<Item>(ITEM_BERRY_SEED), 4);
  character_inv->addItem(std::make_shared<Item>(ITEM_WATERING_CAN), 1);
}

// Draw character to screen
void Character::draw(float x_1, float y_1, float x_2, float y_2) {
  // Indicator
  indicator_x =
      asw::input::mouse.x / map_pointer->VIEWPORT_ZOOM + map_pointer->getX();
  indicator_y =
      asw::input::mouse.y / map_pointer->VIEWPORT_ZOOM + map_pointer->getY();

  // Cursor
  asw::draw::rectFill(indicator_x - map_pointer->getX(),
                      indicator_y - map_pointer->getY(), 2, 2,
                      asw::util::makeColor(255, 255, 255));

  indicator_x -= indicator_x % 16;
  indicator_y -= indicator_y % 16;

  // Only item if hands arent empty
  asw::draw::sprite(indicator, indicator_x - map_pointer->getX(),
                    indicator_y - map_pointer->getY());

  // Draw frame
  asw::draw::stretchSpriteBlit(
      image, floor(ani_ticker / 4) * 16, (direction - 1) * 20, 16, 20,
      x - map_pointer->getX(), y - map_pointer->getY() - 8, 16, 20);

  // Selected item
  if (character_inv->getStack(selected_item)->getItem()) {
    character_inv->getStack(selected_item)
        ->getItem()
        ->draw(x - map_pointer->getX(), y - map_pointer->getY());
  }
}

// Update player
void Character::drawInventory() {
  auto screen_size = asw::display::getLogicalSize();
  const int draw_x = (screen_size.x - HOTBAR_SIZE * 18) / 2;
  const int draw_y = screen_size.y - 20;

  // Draw items
  for (int i = 0; i < HOTBAR_SIZE; i++) {
    asw::draw::sprite(inventory_gui, 18 * i + draw_x, draw_y);
    if (i == selected_item)
      asw::draw::sprite(indicator, 18 * i + 1 + draw_x, 1 + draw_y);
    if (character_inv->getStack(i)) {
      character_inv->getStack(i)->draw(18 * i + 1 + draw_x, 1 + draw_y);
      if (i == selected_item && character_inv->getStack(i)->getItem()) {
        asw::draw::text(
            pixelart, character_inv->getStack(i)->getItem()->getName(),
            1 + draw_x, 21 + draw_y, asw::util::makeColor(255, 255, 255));
      }
    }
  }

  // Draw UI
  if (ui_open && attatched_ui) {
    attatched_ui->draw();
  }
}

// Update player
void Character::update() {
  // Open UI
  if (asw::input::keyboard.pressed[SDL_SCANCODE_E]) {
    if (ui_open) {
      ui_open = false;
      attatched_ui = nullptr;
    } else {
      ui_open = true;
      attatched_ui = inventory_ui;
    }
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_G]) {
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
    attatched_ui->update();
    return;
  }

  // Oh
  // Snap
  if (x % 16 == 0 && y % 16 == 0) {
    moving = false;
  }

  // Selector
  if (asw::input::keyboard.pressed[SDL_SCANCODE_Z] || asw::input::mouse.z > 0) {
    selected_item = (selected_item + (HOTBAR_SIZE - 1)) % HOTBAR_SIZE;
  }
  if (asw::input::keyboard.pressed[SDL_SCANCODE_X] || asw::input::mouse.z < 0) {
    selected_item = (selected_item + 1) % HOTBAR_SIZE;
  }

  // Movement code
  // Move
  if (!moving) {
    // Up
    if (asw::input::keyboard.down[SDL_SCANCODE_UP] ||
        asw::input::keyboard.down[SDL_SCANCODE_W]) {
      direction = DIR_UP;
      if (!map_pointer->world_map->isSolidAt(x, y - 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Down
    else if (asw::input::keyboard.down[SDL_SCANCODE_DOWN] ||
             asw::input::keyboard.down[SDL_SCANCODE_S]) {
      direction = DIR_DOWN;
      if (!map_pointer->world_map->isSolidAt(x, y + 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Left
    else if (asw::input::keyboard.down[SDL_SCANCODE_LEFT] ||
             asw::input::keyboard.down[SDL_SCANCODE_A]) {
      direction = DIR_LEFT;
      if (!map_pointer->world_map->isSolidAt(x - 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Right
    else if (asw::input::keyboard.down[SDL_SCANCODE_RIGHT] ||
             asw::input::keyboard.down[SDL_SCANCODE_D]) {
      direction = DIR_RIGHT;
      if (!map_pointer->world_map->isSolidAt(x + 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }

    // Walk sounds
    if (moving) {
      asw::sound::play(step[sound_step], 50, 125);
    }
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
  auto itemAtPos = map_pointer->world_map->getItemAt(roundf(x / 16.0f) * 16,
                                                     roundf(y / 16.0f) * 16);

  // Pickup
  if (itemAtPos != nullptr) {
    asw::sound::play(pickup);
    if (character_inv->addItem(itemAtPos->itemPtr, 1)) {
      map_pointer->world_map->removeItem(itemAtPos);
    }
  }

  // Drop
  if (asw::input::keyboard.pressed[SDL_SCANCODE_F] ||
      asw::input::mouse.pressed[3]) {
    std::shared_ptr<Item> itemInHand = nullptr;
    if (character_inv->getStack(selected_item)->getItem()) {
      itemInHand = character_inv->getStack(selected_item)->getItem();
    }

    // Drop
    if (itemInHand != nullptr) {
      asw::sound::play(drop);
      map_pointer->world_map->placeItemAt(itemInHand, indicator_x, indicator_y);
      character_inv->getStack(selected_item)->remove(1);
    }
  }

  // Interact with map
  if (asw::input::keyboard.pressed[SDL_SCANCODE_SPACE] ||
      asw::input::mouse.pressed[1]) {
    if (character_inv->getStack(selected_item)->getItem()) {
      map_pointer->interact(indicator_x, indicator_y,
                            character_inv->getStack(selected_item)->getItem());
    }
  }

  // Scroll map
  map_pointer->scroll(x, y);
}
