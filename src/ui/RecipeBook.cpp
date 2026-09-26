#include "RecipeBook.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <format>

#include "../GameState.h"
#include "../Item.h"
#include "../World.h"
#include "../manager/ItemTypeManager.h"
#include "../manager/RecipeManager.h"
#include "../manager/SoundManager.h"
#include "Tooltip.h"
#include "UiScale.h"
#include "../utility/Fonts.h"

namespace {
// Tabs in the order the player gets the stations, names match
// RecipeManager::stationName
constexpr std::array<const char*, 5> TAB_NAMES = {
    "Hand", "Workbench", "Stone workbench", "Furnace", "Kiln"};
constexpr std::array<const char*, 5> TAB_LABELS = {"Hand", "Bench", "Stone",
                                                   "Furnace", "Kiln"};
constexpr int TAB_COUNT = static_cast<int>(TAB_NAMES.size());

// Book size in ui units
constexpr float BOOK_WIDTH = 440.0F;
constexpr float BOOK_HEIGHT = 264.0F;
constexpr float PAGE_WIDTH = 208.0F;
constexpr float TAB_WIDTH = 70.0F;
constexpr float TAB_HEIGHT = 14.0F;

// Recipe rows
constexpr int ROWS_PER_PAGE = 8;
constexpr float ROW_HEIGHT = 26.0F;
constexpr float SLOT = 18.0F;
constexpr float SLOT_GAP = 22.0F;

const asw::Color COVER(96, 58, 34);
const asw::Color COVER_EDGE(60, 35, 20);
const asw::Color PAGE(236, 222, 186);
const asw::Color PAGE_SHADE(214, 196, 152);
const asw::Color TAB(190, 160, 110);
const asw::Color INK(70, 48, 28);
const asw::Color FADED(160, 140, 105);
const asw::Color ENOUGH(40, 120, 40);
const asw::Color SHORT(165, 50, 40);

asw::Font& font() {
  static asw::Font loaded = fonts::load();
  return loaded;
}

bool contains(const asw::Quadf& area, const asw::Vec2i& point) {
  return point.x >= area.position.x &&
         point.x < area.position.x + area.size.x &&
         point.y >= area.position.y && point.y < area.position.y + area.size.y;
}

std::string capitalise(std::string text) {
  if (!text.empty()) {
    text[0] = static_cast<char>(std::toupper(text[0]));
  }
  return text;
}

bool known(const GameState& state, const Recipe& recipe) {
  return std::ranges::any_of(recipe.inputs, [&](auto const& input) {
    return state.known_items.contains(input.first);
  });
}
}  // namespace

void RecipeBook::open(int new_tab) {
  is_open = true;
  tab = std::clamp(new_tab, 0, TAB_COUNT - 1);
  spread = 0;
  SoundManager::play("page");
}

int RecipeBook::tabForStation(const std::string& window, int tier) {
  if (window == "furnace") {
    return 3;
  }
  if (window == "kiln") {
    return 4;
  }
  if (window == "crafting") {
    return std::clamp(tier, 0, 2);
  }
  return 0;
}

RecipeBook::Layout RecipeBook::layout(const asw::Vec2i& ui_size) {
  Layout result;
  const float x = (ui_size.x - BOOK_WIDTH) / 2.0F;
  const float y = (ui_size.y - BOOK_HEIGHT) / 2.0F + TAB_HEIGHT / 2.0F;

  result.book = asw::Quadf(x, y, BOOK_WIDTH, BOOK_HEIGHT);
  result.prev = asw::Quadf(x + 14.0F, y + BOOK_HEIGHT - 20.0F, 44.0F, 12.0F);
  result.next = asw::Quadf(x + BOOK_WIDTH - 58.0F, y + BOOK_HEIGHT - 20.0F,
                           44.0F, 12.0F);

  for (int i = 0; i < TAB_COUNT; i++) {
    result.tabs.emplace_back(x + 14.0F + i * (TAB_WIDTH + 4.0F),
                             y - TAB_HEIGHT + 2.0F, TAB_WIDTH, TAB_HEIGHT);
  }

  return result;
}

std::vector<const Recipe*> RecipeBook::tabRecipes() const {
  std::vector<const Recipe*> found;
  for (auto const& recipe : RecipeManager::getRecipes()) {
    if (RecipeManager::stationName(recipe) == TAB_NAMES[tab]) {
      found.push_back(&recipe);
    }
  }
  return found;
}

int RecipeBook::spreadCount() const {
  const int per_spread = ROWS_PER_PAGE * 2;
  const int count = static_cast<int>(tabRecipes().size());
  return std::max(1, (count + per_spread - 1) / per_spread);
}

void RecipeBook::turn(int delta) {
  const int next = std::clamp(spread + delta, 0, spreadCount() - 1);
  if (next != spread) {
    spread = next;
    SoundManager::play("page");
  }
}

void RecipeBook::update() {
  if (!is_open) {
    return;
  }

  const auto area = layout(getUiSize());
  const auto mouse = getUiMouse();

  if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
    for (int i = 0; i < TAB_COUNT; i++) {
      if (contains(area.tabs[i], mouse) && i != tab) {
        open(i);
      }
    }

    if (contains(area.prev, mouse)) {
      turn(-1);
    } else if (contains(area.next, mouse)) {
      turn(1);
    }
  }

  // Number keys pick a tab, A and D turn pages
  constexpr std::array<asw::input::Key, TAB_COUNT> tab_keys = {
      asw::input::Key::Num1, asw::input::Key::Num2, asw::input::Key::Num3,
      asw::input::Key::Num4, asw::input::Key::Num5};

  for (int i = 0; i < TAB_COUNT; i++) {
    if (asw::input::get_key_down(tab_keys[i]) && i != tab) {
      open(i);
    }
  }

  if (asw::input::get_key_down(asw::input::Key::A) ||
      asw::input::get_key_down(asw::input::Key::Left)) {
    turn(-1);
  }
  if (asw::input::get_key_down(asw::input::Key::D) ||
      asw::input::get_key_down(asw::input::Key::Right)) {
    turn(1);
  }
}

void RecipeBook::drawRecipe(const GameState& state,
                            const Recipe& recipe,
                            const asw::Vec2f& at) const {
  const bool visible = known(state, recipe);
  const auto mouse = getUiMouse();
  auto& bag = World::playerInventory();

  // Item box with a count in the corner, "?" while undiscovered
  auto box = [&](const std::string& id, float x, int count,
                 const asw::Color& count_color) {
    const auto area = asw::Quadf(x, at.y, SLOT, SLOT);
    asw::draw::rect_fill(area, PAGE_SHADE);
    asw::draw::rect(area, FADED);

    if (!visible) {
      asw::draw::text(font(), "?", asw::Vec2f(x + SLOT / 2.0F, at.y + 4.0F),
                      FADED, asw::TextJustify::Center);
      return;
    }

    const Item item(id);
    item.draw(asw::Vec2i(static_cast<int>(x) + 1, static_cast<int>(at.y) + 1));

    if (count > 1) {
      asw::draw::text(font(), std::to_string(count),
                      asw::Vec2f(x + SLOT + 1.0F, at.y + SLOT - 7.0F),
                      count_color, asw::TextJustify::Right);
    }

    if (contains(area, mouse)) {
      Tooltip::setItem(item);
    }
  };

  float x = at.x;
  bool can_make = true;

  for (auto const& [id, quantity] : recipe.inputs) {
    const bool enough = bag.count(id) >= quantity;
    can_make = can_make && enough;
    box(id, x, quantity, enough ? ENOUGH : SHORT);

    // Single items short of stock still get a red marker
    if (visible && quantity == 1 && !enough) {
      asw::draw::rect_fill(asw::Quadf(x + SLOT - 4.0F, at.y + SLOT - 4.0F,
                                      3.0F, 3.0F),
                           SHORT);
    }
    x += SLOT_GAP;
  }

  // Arrow to the result
  const float arrow_y = at.y + SLOT / 2.0F;
  asw::draw::line(asw::Vec2f(x, arrow_y), asw::Vec2f(x + 9.0F, arrow_y), INK);
  asw::draw::line(asw::Vec2f(x + 6.0F, arrow_y - 3.0F),
                  asw::Vec2f(x + 9.0F, arrow_y), INK);
  asw::draw::line(asw::Vec2f(x + 6.0F, arrow_y + 3.0F),
                  asw::Vec2f(x + 9.0F, arrow_y), INK);
  x += 14.0F;

  box(recipe.output, x, recipe.count, INK);
  x += SLOT + 5.0F;

  const auto name =
      visible ? capitalise(ItemTypeManager::getItem(recipe.output).getName())
              : "???";
  asw::draw::text(font(), name, asw::Vec2f(x, at.y + 1.0F),
                  visible ? INK : FADED);

  if (visible) {
    asw::draw::text(font(), can_make ? "You have it all" : "Missing items",
                    asw::Vec2f(x, at.y + 10.0F), can_make ? ENOUGH : FADED);
  } else {
    asw::draw::text(font(), "Find one of its items",
                    asw::Vec2f(x, at.y + 10.0F), FADED);
  }
}

void RecipeBook::draw(const GameState& state,
                      const asw::Vec2i& ui_size) const {
  if (!is_open) {
    return;
  }

  const auto area = layout(ui_size);
  const auto& book = area.book;
  const float bx = book.position.x;
  const float by = book.position.y;

  asw::display::set_blend_mode(asw::BlendMode::Blend);
  asw::draw::rect_fill(asw::Quadf(0, 0, ui_size.x, ui_size.y),
                       asw::Color(0, 0, 0, 120));

  // Tabs, the open one joins its page
  for (int i = 0; i < TAB_COUNT; i++) {
    const auto& t = area.tabs[i];
    asw::draw::rect_fill(t, i == tab ? PAGE : TAB);
    asw::draw::rect(t, COVER_EDGE);
    asw::draw::text(font(), TAB_LABELS[i],
                    asw::Vec2f(t.position.x + TAB_WIDTH / 2.0F,
                               t.position.y + 3.0F),
                    INK, asw::TextJustify::Center);
  }

  // Cover and pages
  asw::draw::rect_fill(book, COVER);
  asw::draw::rect(book, COVER_EDGE);

  const float page_y = by + 6.0F;
  const float page_h = BOOK_HEIGHT - 12.0F;
  const std::array<float, 2> page_x = {bx + 8.0F, bx + 224.0F};

  for (auto const px : page_x) {
    asw::draw::rect_fill(asw::Quadf(px, page_y, PAGE_WIDTH, page_h), PAGE);
  }

  // Spine shading
  asw::draw::rect_fill(asw::Quadf(page_x[0] + PAGE_WIDTH - 6.0F, page_y, 6.0F,
                                  page_h),
                       PAGE_SHADE);
  asw::draw::rect_fill(asw::Quadf(page_x[1], page_y, 6.0F, page_h),
                       PAGE_SHADE);
  asw::draw::line(asw::Vec2f(bx + BOOK_WIDTH / 2.0F, page_y),
                  asw::Vec2f(bx + BOOK_WIDTH / 2.0F, page_y + page_h),
                  COVER_EDGE);

  // Open tab covers the join with the cover
  const auto& open_tab = area.tabs[tab];
  asw::draw::rect_fill(asw::Quadf(open_tab.position.x + 1.0F, by,
                                  TAB_WIDTH - 2.0F, 6.0F),
                       PAGE);

  // Headings
  const auto recipes = tabRecipes();
  const int total = static_cast<int>(recipes.size());
  const int found = static_cast<int>(std::ranges::count_if(
      recipes, [&](const Recipe* r) { return known(state, *r); }));

  asw::draw::text(font(), TAB_NAMES[tab],
                  asw::Vec2f(page_x[0] + 10.0F, page_y + 6.0F), INK);
  asw::draw::text(font(), std::format("{}/{} found", found, total),
                  asw::Vec2f(page_x[1] + PAGE_WIDTH - 10.0F, page_y + 6.0F),
                  FADED, asw::TextJustify::Right);

  for (auto const px : page_x) {
    asw::draw::line(asw::Vec2f(px + 10.0F, page_y + 17.0F),
                    asw::Vec2f(px + PAGE_WIDTH - 12.0F, page_y + 17.0F),
                    PAGE_SHADE);
  }

  // Recipes, left page then right
  const int first = spread * ROWS_PER_PAGE * 2;
  for (int i = 0; i < ROWS_PER_PAGE * 2 && first + i < total; i++) {
    const int page = i / ROWS_PER_PAGE;
    const int row = i % ROWS_PER_PAGE;
    drawRecipe(state, *recipes[first + i],
               asw::Vec2f(page_x[page] + 12.0F,
                          page_y + 24.0F + row * ROW_HEIGHT));
  }

  // Page turns and help
  const int spreads = spreadCount();
  if (spread > 0) {
    asw::draw::text(font(), "< Back",
                    asw::Vec2f(area.prev.position.x, area.prev.position.y),
                    INK);
  }
  if (spread < spreads - 1) {
    asw::draw::text(font(), "More >",
                    asw::Vec2f(area.next.position.x + area.next.size.x,
                               area.next.position.y),
                    INK, asw::TextJustify::Right);
  }

  asw::draw::text(font(), "1-5 tabs   A D pages   R close",
                  asw::Vec2f(bx + BOOK_WIDTH / 2.0F, by + BOOK_HEIGHT + 4.0F),
                  asw::Color(230, 220, 200), asw::TextJustify::Center);
}
