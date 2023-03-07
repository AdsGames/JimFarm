#include "Character.h"

#include <math.h>

#include "utility/Tools.h"

#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "manager/TileTypeManager.h"

#include "Graphics.h"

// Top of head
CharacterForeground::CharacterForeground(Character* charPtr)
    : Sprite(0, 0, 5), char_ptr(charPtr) {}

void CharacterForeground::draw(int x_1, int y_1, int x_2, int y_2) const {
  auto animation_frame =
      static_cast<int>(floor(char_ptr->ani_ticker / 4.0f)) * 16;

  asw::draw::stretchSpriteBlit(char_ptr->image, animation_frame,
                               (char_ptr->direction - 1) * 20, 16, 8,
                               x - char_ptr->map_pointer->getX(),
                               y - char_ptr->map_pointer->getY() - 8, 16, 8);
}

void CharacterForeground::update() {
  this->x = char_ptr->x;
  this->y = char_ptr->y;
}

// Ctor for character
Character::Character() : Sprite(0, 0, 2) {
  c_fore = std::make_shared<CharacterForeground>(this);
  Graphics::Instance()->add(c_fore);
}

// World object to point to (needs this!)
void Character::setWorld(World* wrld) {
  this->map_pointer = wrld;
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

  inventory_ui.getInventory()->addItem(std::make_shared<Item>(ITEM_AXE), 1);
  inventory_ui.getInventory()->addItem(std::make_shared<Item>(ITEM_SCYTHE), 1);
  inventory_ui.getInventory()->addItem(std::make_shared<Item>(ITEM_SHOVEL), 1);
  inventory_ui.getInventory()->addItem(std::make_shared<Item>(ITEM_HOE), 1);
  inventory_ui.getInventory()->addItem(std::make_shared<Item>(ITEM_BERRY_SEED),
                                       4);
  inventory_ui.getInventory()->addItem(
      std::make_shared<Item>(ITEM_WATERING_CAN), 1);
}

void Character::setPosition(int pos_x, int pos_y) {
  x = pos_x;
  y = pos_y;
}

// Draw character to screen
void Character::draw(int x_1, int y_1, int x_2, int y_2) const {
  const auto cursor_x =
      asw::input::mouse.x / static_cast<int>(map_pointer->getZoom());
  const auto cursor_y =
      asw::input::mouse.y / static_cast<int>(map_pointer->getZoom());

  // Cursor
  asw::draw::rectFill(cursor_x, cursor_y, 2, 2,
                      asw::util::makeColor(255, 255, 255));

  // Only item if hands arent empty
  asw::draw::sprite(indicator, indicator_x - map_pointer->getX(),
                    indicator_y - map_pointer->getY());

  // Draw frame
  asw::draw::stretchSpriteBlit(image, ani_ticker / 4 * 16, (direction - 1) * 20,
                               16, 20, x - map_pointer->getX(),
                               y - map_pointer->getY() - 8, 16, 20);

  // Selected item
  if (inventory_ui.getInventory()->getStack(selected_item)->getItem()) {
    inventory_ui.getInventory()
        ->getStack(selected_item)
        ->getItem()
        ->draw(x - map_pointer->getX(), y - map_pointer->getY());
  }
}

// Update player
void Character::drawInventory() const {
  auto screen_size = asw::display::getLogicalSize();
  const int draw_x = (screen_size.x - HOTBAR_SIZE * 18) / 2;
  const int draw_y = screen_size.y - 20;

  // Draw items
  for (int i = 0; i < HOTBAR_SIZE; i++) {
    asw::draw::sprite(inventory_gui, 18 * i + draw_x, draw_y);
    if (i == selected_item) {
      asw::draw::sprite(indicator, 18 * i + 1 + draw_x, 1 + draw_y);
    }
    if (inventory_ui.getInventory()->getStack(i)) {
      inventory_ui.getInventory()->getStack(i)->draw(18 * i + 1 + draw_x,
                                                     1 + draw_y);
      if (i == selected_item &&
          inventory_ui.getInventory()->getStack(i)->getItem()) {
        asw::draw::text(
            pixelart,
            inventory_ui.getInventory()->getStack(i)->getItem()->getName(),
            1 + draw_x, 21 + draw_y, asw::util::makeColor(255, 255, 255));
      }
    }
  }

  // Draw UI
  if (ui_open && attached_ui != -1) {
    auto& attached_ui_instance =
        InterfaceTypeManager::getInterfaceById(attached_ui);
    attached_ui_instance.draw();
  }
}

// Update player
void Character::update() {
  const auto relative_x =
      asw::input::mouse.x / static_cast<int>(map_pointer->getZoom()) +
      map_pointer->getX();
  const auto relative_y =
      asw::input::mouse.y / static_cast<int>(map_pointer->getZoom()) +
      map_pointer->getY();

  // Indicator
  indicator_x = relative_x - (relative_x % 16);
  indicator_y = relative_y - (relative_y % 16);

  // Open UI
  if (asw::input::keyboard.pressed[SDL_SCANCODE_E]) {
    if (ui_open) {
      ui_open = false;
      attached_ui = -1;
    } else {
      ui_open = true;
      attached_ui = 0;
    }
  }

  if (asw::input::keyboard.pressed[SDL_SCANCODE_G]) {
    if (ui_open) {
      ui_open = false;
      attached_ui = -1;
    } else {
      ui_open = true;
      attached_ui = 1;
    }
  }

  // UI Open
  if (ui_open && attached_ui != -1) {
    // Update UI
    auto& attached_ui_instance =
        InterfaceTypeManager::getInterfaceById(attached_ui);
    attached_ui_instance.update();
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
      if (!map_pointer->getMap().isSolidAt(x, y - 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Down
    else if (asw::input::keyboard.down[SDL_SCANCODE_DOWN] ||
             asw::input::keyboard.down[SDL_SCANCODE_S]) {
      direction = DIR_DOWN;
      if (!map_pointer->getMap().isSolidAt(x, y + 16)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Left
    else if (asw::input::keyboard.down[SDL_SCANCODE_LEFT] ||
             asw::input::keyboard.down[SDL_SCANCODE_A]) {
      direction = DIR_LEFT;
      if (!map_pointer->getMap().isSolidAt(x - 16, y)) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Right
    else if (asw::input::keyboard.down[SDL_SCANCODE_RIGHT] ||
             asw::input::keyboard.down[SDL_SCANCODE_D]) {
      direction = DIR_RIGHT;
      if (!map_pointer->getMap().isSolidAt(x + 16, y)) {
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
             y < (map_pointer->getMap().getHeight() * 16) - 16)
      y += 2;
    else if (direction == DIR_LEFT && x > 0)
      x -= 2;
    else if (direction == DIR_RIGHT &&
             x < (map_pointer->getMap().getWidth() * 16) - 16)
      x += 2;

    // Increase animation ticker
    ani_ticker = (ani_ticker + 1) % 16;
  } else {
    ani_ticker = 0;
  }

  // Pickup
  auto item_at_position =
      map_pointer->getMap().getItemAt(x / 16 * 16, y / 16 * 16);

  // Pickup
  if (item_at_position != nullptr) {
    asw::sound::play(pickup);
    if (inventory_ui.getInventory()->addItem(item_at_position->itemPtr, 1)) {
      map_pointer->getMap().removeItem(item_at_position);
    }
  }

  // Drop
  if (asw::input::keyboard.pressed[SDL_SCANCODE_F] ||
      asw::input::mouse.pressed[3]) {
    std::shared_ptr<Item> itemInHand = nullptr;
    if (inventory_ui.getInventory()->getStack(selected_item)->getItem()) {
      itemInHand =
          inventory_ui.getInventory()->getStack(selected_item)->getItem();
    }

    // Drop
    if (itemInHand != nullptr) {
      asw::sound::play(drop);
      map_pointer->getMap().placeItemAt(itemInHand, indicator_x, indicator_y);
      inventory_ui.getInventory()->getStack(selected_item)->remove(1);
    }
  }

  // Interact with map
  if ((asw::input::keyboard.pressed[SDL_SCANCODE_SPACE] ||
       asw::input::mouse.pressed[1]) &&
      inventory_ui.getInventory()->getStack(selected_item)->getItem()) {
    map_pointer->interact(
        indicator_x, indicator_y,
        inventory_ui.getInventory()->getStack(selected_item)->getItem());
  }

  // Scroll map
  map_pointer->scroll(x, y);

  this->c_fore->update();
}
