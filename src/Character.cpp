#include "Character.h"

#include "Graphics.h"
#include "manager/TileTypeManager.h"
#include "ui/Tooltip.h"
#include "ui/UiScale.h"
#include "utility/Tools.h"

// Top of head
CharacterForeground::CharacterForeground(Character* charPtr)
    : Sprite(), char_ptr(charPtr) {
  this->z = 2.5f;
}

void CharacterForeground::draw(const Camera& camera) const {
  auto animation_frame =
      static_cast<int>(floor(char_ptr->ani_ticker / 4.0f)) * 16;

  asw::draw::stretch_sprite_blit(
      char_ptr->image,
      asw::Quadf(animation_frame, (char_ptr->direction - 1) * 20, 16, 8),
      asw::Quadf(pos.x - camera.getPosition().x,
                 pos.y - camera.getPosition().y - 8, 16, 8));
}

void CharacterForeground::update() {
  this->pos = char_ptr->pos;
}

// Ctor for character
Character::Character() : Sprite() {
  this->z = 2.0f;
  c_fore = std::make_shared<CharacterForeground>(this);
  Graphics::Instance().add(c_fore, true);
}

// Set image
void Character::loadData() {
  // Images
  image = asw::assets::load_texture("assets/images/character_1.png");
  inventory_gui = asw::assets::load_texture("assets/images/GUI_INVENTORY.png");
  indicator = asw::assets::load_texture("assets/images/indicator.png");
  coin = asw::assets::load_texture("assets/images/coin.png");

  // Sounds
  pickup = asw::assets::load_sample("assets/sfx/pickup.wav");
  drop = asw::assets::load_sample("assets/sfx/drop.wav");
  step[0] = asw::assets::load_sample("assets/sfx/step_1.wav");
  step[1] = asw::assets::load_sample("assets/sfx/step_2.wav");

  // Load fonts
  pixelart = asw::assets::load_font("assets/fonts/pixelart.ttf", 12);
}

void Character::giveStarterItems() {
  auto inventory = inventory_ui.getInventory();
  inventory->addItem(std::make_shared<Item>("item:hoe"), 1);
  inventory->addItem(std::make_shared<Item>("item:watering_can"), 1);
  inventory->addItem(std::make_shared<Item>("item:berry_seed"), 6);
  inventory->addItem(std::make_shared<Item>("item:carrot_seed"), 4);
  inventory->addItem(std::make_shared<Item>("item:axe"), 1);
  inventory->addItem(std::make_shared<Item>("item:scythe"), 1);
  inventory->addItem(std::make_shared<Item>("item:shovel"), 1);
  inventory->addItem(std::make_shared<Item>("item:berry"), 4);
}

void Character::setPosition(const asw::Vec2i& pos) {
  this->pos = pos;
}

// Draw character to screen
void Character::draw(const Camera& camera) const {
  const auto& mouse = asw::input::get_mouse();

  const auto cursor_x =
      static_cast<int>(static_cast<float>(mouse.position.x) / camera.getZoom());
  const auto cursor_y =
      static_cast<int>(static_cast<float>(mouse.position.y) / camera.getZoom());

  // Cursor
  asw::draw::rect_fill(asw::Quadf(cursor_x, cursor_y, 2, 2),
                       asw::Color(255, 255, 255));

  // Only item if hands arent empty
  asw::draw::sprite(indicator,
                    asw::Vec2f(indicator_pos.x - camera.getPosition().x,
                               indicator_pos.y - camera.getPosition().y));

  // Draw frame
  asw::draw::stretch_sprite_blit(
      image, asw::Quadf(ani_ticker / 4 * 16, (direction - 1) * 20, 16, 20),
      asw::Quadf(pos.x - camera.getPosition().x,
                 pos.y - camera.getPosition().y - 8, 16, 20));

  // Selected item
  if (inventory_ui.getInventory()->getStack(selected_item)->getItem()) {
    inventory_ui.getInventory()
        ->getStack(selected_item)
        ->getItem()
        ->draw(pos - camera.getPosition());
  }
}

// Update player
void Character::drawInventory(const asw::Vec2i& screen_size) const {
  const int draw_x = (screen_size.x - HOTBAR_SIZE * 18) / 2;
  const int draw_y = screen_size.y - 20;

  // Draw items
  for (int i = 0; i < HOTBAR_SIZE; i++) {
    asw::draw::sprite(inventory_gui, asw::Vec2f(18 * i + draw_x, draw_y));

    if (i == selected_item) {
      asw::draw::sprite(indicator, asw::Vec2f(18 * i + 1 + draw_x, 1 + draw_y));
    }

    auto stack = inventory_ui.getInventory()->getStack(i);

    if (stack) {
      stack->draw(asw::Vec2i(18 * i + 1 + draw_x, 1 + draw_y));
    }
  }

  // Tooltip for the hotbar slot under the cursor
  const auto mouse = getUiMouse();
  const int slot = (mouse.x - draw_x) / 18;
  if (!UiController::isHoldingItem() && mouse.x >= draw_x &&
      mouse.y >= draw_y && mouse.y < draw_y + 18 && slot < HOTBAR_SIZE) {
    auto stack = inventory_ui.getInventory()->getStack(slot);
    if (stack && stack->getItem()) {
      Tooltip::setItem(*stack->getItem());
    }
  }
}

std::shared_ptr<Item> Character::getSelectedItem() const {
  return inventory_ui.getInventory()->getStack(selected_item)->getItem();
}

// Update player
void Character::update(World& world) {
  const auto& mouse = asw::input::get_mouse();

  auto relative_x = static_cast<int>(static_cast<float>(mouse.position.x) /
                                     world.getCamera().getZoom()) +
                    world.getCamera().getPosition().x;
  auto relative_y = static_cast<int>(static_cast<float>(mouse.position.y) /
                                     world.getCamera().getZoom()) +
                    world.getCamera().getPosition().y;

  auto tile_index = pos / TILE_SIZE;

  // Open windows take the input
  const bool input_enabled = !world.getHud().isOpen();

  // Indicator
  indicator_pos = asw::Vec2i(relative_x - (relative_x % 16),
                             relative_y - (relative_y % 16));

  // Oh
  // Snap
  if (pos.x % TILE_SIZE == 0 && pos.y % TILE_SIZE == 0) {
    moving = false;
  }

  // Selector
  if (input_enabled &&
      (asw::input::get_key_down(asw::input::Key::Z) || mouse.z > 0)) {
    selected_item = (selected_item + (HOTBAR_SIZE - 1)) % HOTBAR_SIZE;
  }
  if (input_enabled &&
      (asw::input::get_key_down(asw::input::Key::X) || mouse.z < 0)) {
    selected_item = (selected_item + 1) % HOTBAR_SIZE;
  }

  // Number keys select hotbar slot directly
  constexpr std::array<asw::input::Key, HOTBAR_SIZE> hotbar_keys = {
      asw::input::Key::Num1, asw::input::Key::Num2, asw::input::Key::Num3,
      asw::input::Key::Num4, asw::input::Key::Num5, asw::input::Key::Num6,
      asw::input::Key::Num7, asw::input::Key::Num8};

  for (int i = 0; i < HOTBAR_SIZE; i++) {
    if (input_enabled && asw::input::get_key_down(hotbar_keys[i])) {
      selected_item = i;
    }
  }

  // Movement code, speed is chosen per step so it stays on the tile grid
  const auto& state = world.getState();
  if (!moving) {
    move_speed =
        (state.energy <= 0.0F || state.health < 25.0F || state.warmth <= 0.0F)
            ? 1
            : 2;
  }

  // Move
  if (!moving && input_enabled) {
    // Up
    if (asw::input::get_key(asw::input::Key::Up) ||
        asw::input::get_key(asw::input::Key::W)) {
      direction = DIR_UP;
      if (!world.getMap().isSolidAt(tile_index + asw::Vec2i(0, -1))) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Down
    else if (asw::input::get_key(asw::input::Key::Down) ||
             asw::input::get_key(asw::input::Key::S)) {
      direction = DIR_DOWN;
      if (!world.getMap().isSolidAt(tile_index + asw::Vec2i(0, 1))) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Left
    else if (asw::input::get_key(asw::input::Key::Left) ||
             asw::input::get_key(asw::input::Key::A)) {
      direction = DIR_LEFT;
      if (!world.getMap().isSolidAt(tile_index + asw::Vec2i(-1, 0))) {
        moving = true;
        sound_step = !sound_step;
      }
    }
    // Right
    else if (asw::input::get_key(asw::input::Key::Right) ||
             asw::input::get_key(asw::input::Key::D)) {
      direction = DIR_RIGHT;
      if (!world.getMap().isSolidAt(tile_index + asw::Vec2i(1, 0))) {
        moving = true;
        sound_step = !sound_step;
      }
    }

    // Walk sounds
    if (moving) {
      asw::sound::play(step[sound_step], 0.2F);
    }
  }

  // Update movement
  if (moving) {
    // Smooth move
    if (direction == DIR_UP && pos.y > 0) {
      pos.y -= move_speed;
    } else if (direction == DIR_DOWN &&
               pos.y < (world.getMap().getHeight() * TILE_SIZE) - TILE_SIZE) {
      pos.y += move_speed;
    } else if (direction == DIR_LEFT && pos.x > 0) {
      pos.x -= move_speed;
    } else if (direction == DIR_RIGHT &&
               pos.x < (world.getMap().getWidth() * TILE_SIZE) - TILE_SIZE) {
      pos.x += move_speed;
    }

    // Increase animation ticker
    ani_ticker = (ani_ticker + 1) % 16;
  } else {
    ani_ticker = 0;
  }

  // Pickup
  auto item_at_position = world.getMap().getItemAt(tile_index);

  // Pickup
  if (item_at_position != nullptr) {
    asw::sound::play(pickup);
    if (inventory_ui.getInventory()->addItem(item_at_position->itemPtr, 1)) {
      world.getMap().removeItem(item_at_position);
    }
  }

  // Drop
  if (input_enabled && asw::input::get_key_down(asw::input::Key::F)) {
    std::shared_ptr<Item> itemInHand = nullptr;
    if (inventory_ui.getInventory()->getStack(selected_item)->getItem()) {
      itemInHand =
          inventory_ui.getInventory()->getStack(selected_item)->getItem();
    }

    // Drop
    if (itemInHand != nullptr) {
      asw::sound::play(drop);
      world.getMap().placeItemAt(itemInHand, indicator_pos / TILE_SIZE);
      inventory_ui.getInventory()->getStack(selected_item)->remove(1);
    }
  }

  // Interact with map, empty hands can pick, drink and harvest
  auto held = inventory_ui.getInventory()->getStack(selected_item);

  if (input_enabled &&
      (asw::input::get_key_down(asw::input::Key::Space) ||
       asw::input::get_mouse_button_down(asw::input::MouseButton::Left))) {
    world.interact(indicator_pos, pos, *held);
  }

  // Eat or drink
  if (input_enabled &&
      (asw::input::get_key_down(asw::input::Key::C) ||
       asw::input::get_mouse_button_down(asw::input::MouseButton::Right))) {
    world.consume(*held);
  }

  // Scroll map
  world.getCamera().pan(pos - world.getCamera().getSize() / 2);

  this->c_fore->update();
}
