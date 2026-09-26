#include "Game.h"

#include <algorithm>
#include <array>
#include <format>

#include "GameMenu.h"
#include "Graphics.h"
#include "Menu.h"
#include "SaveManager.h"
#include "Tile.h"
#include "manager/ItemTypeManager.h"
#include "manager/RecipeManager.h"
#include "ui/Tooltip.h"
#include "ui/UiScale.h"
#include "utility/Fonts.h"
#include "utility/Tools.h"

void Game::init() {
  const auto action = std::exchange(pending_game_action, GameAction::Resume);

  // Back from the pause menu
  if (initialized && action == GameAction::Resume) {
    return;
  }

  World::loadData();

  if (!ui_buffer) {
    const auto ui_size = getUiSize();
    ui_buffer = asw::assets::create_texture(ui_size.x, ui_size.y);

    // Blending into a transparent buffer leaves colours premultiplied by alpha
    asw::draw::set_blend_mode(ui_buffer, asw::BlendMode::BlendPremultiplied);

    font = fonts::load();
  }

  // Setup jim
  jim = std::make_shared<Character>();
  jim->loadData();
  Graphics::Instance().add(jim, true);

  const bool loaded =
      action == GameAction::Continue && SaveManager::load(farm_world, *jim);

  if (!loaded) {
    farm_world.newGame();
    jim->giveStarterItems();
    jim->setPosition(farm_world.getState().home * TILE_SIZE);
  }

  save_current_game = [this]() { return save(); };
  initialized = true;
}

bool Game::save() {
  if (!initialized) {
    return false;
  }

  const bool saved = SaveManager::save(farm_world, *jim);
  farm_world.getMessenger().pushMessage(saved ? "Game saved"
                                              : "Could not save the game");
  return saved;
}

void Game::update(float dt) {
  // Summary waits for the player
  if (farm_world.isSummaryOpen() &&
      (asw::input::get_key_down(asw::input::Key::Space) ||
       asw::input::get_key_down(asw::input::Key::Return) ||
       asw::input::get_mouse_button_down(asw::input::MouseButton::Left))) {
    farm_world.closeSummary();
    return;
  }

  // Update world, windows under the recipe book ignore clicks
  farm_world.getHud().setBlocked(recipe_book.isOpen());
  farm_world.update(dt, jim->getPosition());

  // Sleeping and fainting send jim home
  asw::Vec2i warp;
  if (farm_world.takePlayerWarp(warp)) {
    jim->setPosition(warp);
  }

  // Autosave each morning
  if (farm_world.takeSaveRequest()) {
    save();
  }

  if (asw::input::get_key_down(asw::input::Key::H)) {
    show_help = !show_help;
    recipe_book.close();
  }

  // Recipe book opens at the station in use
  if (asw::input::get_key_down(asw::input::Key::R)) {
    show_help = false;
    if (recipe_book.isOpen()) {
      recipe_book.close();
    } else {
      auto& hud = farm_world.getHud();
      const auto& state = farm_world.getState();
      int tab = RecipeBook::tabForStation("", 0);
      for (const auto* window : {"crafting", "furnace", "kiln"}) {
        if (hud.isOpen(window)) {
          tab = RecipeBook::tabForStation(window, state.crafting_tier);
        }
      }
      recipe_book.open(tab);
    }
  } else {
    recipe_book.update();
  }

  // Update character, the open book takes the input
  if (!farm_world.isSummaryOpen() && !recipe_book.isOpen()) {
    jim->update(farm_world, dt);
  }

  // Close windows, else go to menu
  if (asw::input::get_key_down(asw::input::Key::Escape)) {
    if (show_help || recipe_book.isOpen()) {
      show_help = false;
      recipe_book.close();
    } else if (farm_world.getHud().isOpen()) {
      farm_world.getHud().closeAll();
    } else {
      manager.set_next_scene(ProgramState::GAME_MENU);
    }
  }
}

void Game::drawHelp(const asw::Vec2i& ui_size) const {
  std::vector<std::string> lines = {
      "CONTROLS",
      "WASD / Arrows  move     Left click / Space  use held item",
      "Right click / C  open, harvest, pick up, drink, talk",
      "Hold right click / C with food to eat it",
      "1-8 / Z X / wheel  select hotbar",
      "F  drop       E  bag       Q  crafting      G  furnace",
      "R  recipe book, click tabs or 1-5, A D turn pages",
      "The words by the cursor show what each click does",
      "Right click a bed to sleep, the store to trade",
      "Right click a workbench or kiln to craft with it",
      "Walls, windows and a door make a warm room",
      "Doors open as you walk in, right click holds them open",
      "",
      "WINDOWS",
      "Click: take or swap   Right click: half, or place one",
      "Drag: spread evenly   Right drag: one in each slot",
      "Shift click: move across, or craft all from the output",
      "Double click: collect all   1-8: swap with hotbar",
      "F: drop one, Ctrl F all   Click outside: throw held items",
      "+ / -  zoom    H  help    Esc  close / menu",
  };

  const int width = 300;
  const int height = static_cast<int>(lines.size()) * 10 + 12;
  const int x = (ui_size.x - width) / 2;
  const int y = std::max(4, (ui_size.y - height) / 2);

  asw::draw::rect_fill(asw::Quadf(x, y, width, height),
                       asw::Color(20, 25, 35, 235));
  asw::draw::rect(asw::Quadf(x, y, width, height), asw::Color(120, 150, 200));

  for (size_t i = 0; i < lines.size(); i++) {
    asw::draw::text(font, lines[i],
                    asw::Vec2f(x + 8, y + 6 + static_cast<int>(i) * 10),
                    asw::Color(255, 255, 255));
  }
}

void Game::drawHoverVerbs() {
  if (farm_world.getHud().isOpen() || recipe_book.isOpen() ||
      farm_world.isSummaryOpen() || show_help) {
    return;
  }

  const auto verbs = farm_world.hoverVerbs(
      jim->getCursorTile(), jim->getPosition(), *jim->getHeldStack());

  std::vector<std::pair<std::string, asw::Color>> lines;
  if (!verbs.left.empty()) {
    lines.emplace_back("L " + verbs.left, asw::Color(255, 255, 255));
  }
  if (!verbs.right.empty()) {
    lines.emplace_back("R " + verbs.right, asw::Color(200, 230, 160));
  }
  if (lines.empty()) {
    return;
  }

  const auto mouse = getUiMouse();
  int width = 0;
  for (auto const& [text, _] : lines) {
    width = std::max(width, asw::util::get_text_size(font, text).x);
  }

  const float x = static_cast<float>(mouse.x + 8);
  const float y = static_cast<float>(mouse.y + 8);
  asw::draw::rect_fill(
      asw::Quadf(x - 2.0F, y - 1.0F, static_cast<float>(width + 4),
                 static_cast<float>(lines.size() * 9 + 1)),
      asw::Color(0, 0, 0, 140));

  for (size_t i = 0; i < lines.size(); i++) {
    asw::draw::text(font, lines[i].first,
                    asw::Vec2f(x, y + static_cast<float>(i * 9)),
                    lines[i].second);
  }
}

void Game::draw() {
  // Draw map
  farm_world.draw();

  // Draw ui at 1x into buffer. Size is read first, logical size is per target
  const auto ui_size = getUiSize();
  asw::display::set_render_target(ui_buffer);
  asw::display::clear(asw::Color(0, 0, 0, 0));

  Tooltip::setSeason(farm_world.getState().getSeason());
  farm_world.drawStatus(ui_size);
  farm_world.getHud().draw(farm_world.getState());
  jim->drawInventory(ui_size);
  drawHoverVerbs();
  recipe_book.draw(farm_world.getState(), ui_size);
  Tooltip::draw(ui_size);
  farm_world.drawSummary(ui_size);

  if (show_help) {
    drawHelp(ui_size);
  }



  asw::display::reset_render_target();

  // Scale ui up to screen
  const auto screen_size = asw::display::get_logical_size();
  asw::draw::stretch_sprite(ui_buffer,
                            asw::Quadf(0, 0, screen_size.x, screen_size.y));
}
