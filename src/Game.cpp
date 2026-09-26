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
#include "utility/Tools.h"

namespace {
constexpr size_t RECIPES_PER_PAGE = 24;
}  // namespace

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

    font = asw::assets::load_font("assets/fonts/pixelart.ttf", 8,
                                  asw::FontStyle::Pixel);
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

  // Update world
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
    recipe_page = -1;
  }

  // Each press turns the page, closing after the last
  if (asw::input::get_key_down(asw::input::Key::R)) {
    show_help = false;
    const int pages = static_cast<int>(
        (recipeLines().size() + RECIPES_PER_PAGE - 1) / RECIPES_PER_PAGE);
    recipe_page = recipe_page + 1 >= pages ? -1 : recipe_page + 1;
  }

  // Update character
  if (!farm_world.isSummaryOpen()) {
    jim->update(farm_world);
  }

  // Close windows, else go to menu
  if (asw::input::get_key_down(asw::input::Key::Escape)) {
    if (show_help || recipe_page >= 0) {
      show_help = false;
      recipe_page = -1;
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
      "WASD / Arrows  move        Click / Space  use item",
      "1-8 / Z X / wheel  select   C / Right click  eat, drink",
      "F  drop       E  bag       Q  crafting      G  furnace",
      "R  recipe book, press again for the next page",
      "Empty hand: harvest, forage, pick up chickens, drink",
      "Click a bed to sleep, the store to trade",
      "Click a workbench or kiln to craft with it",
      "Walls, windows and a door make a warm room",
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

std::vector<std::string> Game::recipeLines() const {
  const auto& known = farm_world.getState().known_items;
  std::vector<std::string> lines;
  int hidden = 0;

  // Stations in the order the player gets them
  const std::array<std::string, 5> order = {
      "Hand", "Workbench", "Stone workbench", "Furnace", "Kiln"};

  for (auto const& station : order) {
    for (auto const& recipe : RecipeManager::getRecipes()) {
      if (RecipeManager::stationName(recipe) != station) {
        continue;
      }

      // Shown once the player has held one of the inputs
      if (std::ranges::none_of(recipe.inputs, [&](auto const& input) {
            return known.contains(input.first);
          })) {
        hidden++;
        continue;
      }

      std::string inputs;
      for (auto const& [id, quantity] : recipe.inputs) {
        inputs += std::format("{}{} {}", inputs.empty() ? "" : " + ", quantity,
                              ItemTypeManager::getItem(id).getName());
      }

      lines.push_back(std::format(
          "{}: {} -> {} {}", station, inputs, recipe.count,
          ItemTypeManager::getItem(recipe.output).getName()));
    }
  }

  if (hidden > 0) {
    lines.push_back(std::format(
        "{} more to discover, pick up new things to learn them", hidden));
  }

  return lines;
}

void Game::drawRecipes(const asw::Vec2i& ui_size) const {
  const auto all = recipeLines();
  const int pages = static_cast<int>(
      (all.size() + RECIPES_PER_PAGE - 1) / RECIPES_PER_PAGE);

  std::vector<std::string> lines = {
      std::format("RECIPES  page {}/{}  (R next, Esc close)", recipe_page + 1,
                  pages),
      ""};

  const auto first = static_cast<size_t>(recipe_page * RECIPES_PER_PAGE);
  for (size_t i = first; i < all.size() && i < first + RECIPES_PER_PAGE; i++) {
    lines.push_back(all[i]);
  }

  const int width = std::min(ui_size.x - 8, 440);
  const int height = static_cast<int>(lines.size()) * 10 + 12;
  const int x = (ui_size.x - width) / 2;
  const int y = std::max(4, (ui_size.y - height) / 2);

  asw::draw::rect_fill(asw::Quadf(x, y, width, height),
                       asw::Color(35, 28, 20, 235));
  asw::draw::rect(asw::Quadf(x, y, width, height), asw::Color(200, 170, 110));

  for (size_t i = 0; i < lines.size(); i++) {
    asw::draw::text(font, lines[i],
                    asw::Vec2f(x + 8, y + 6 + static_cast<int>(i) * 10),
                    asw::Color(255, 255, 255));
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
  Tooltip::draw(ui_size);
  farm_world.drawSummary(ui_size);

  if (show_help) {
    drawHelp(ui_size);
  }

  if (recipe_page >= 0) {
    drawRecipes(ui_size);
  }

  asw::display::reset_render_target();

  // Scale ui up to screen
  const auto screen_size = asw::display::get_logical_size();
  asw::draw::stretch_sprite(ui_buffer,
                            asw::Quadf(0, 0, screen_size.x, screen_size.y));
}
