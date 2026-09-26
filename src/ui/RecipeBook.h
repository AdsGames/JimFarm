#ifndef SRC_UI_RECIPE_BOOK_H_
#define SRC_UI_RECIPE_BOOK_H_

#include <asw/asw.h>

#include <string>
#include <vector>

class GameState;
struct Recipe;

// Illustrated recipe book, one tab per crafting station. Recipes show once the
// player has held one of their inputs, the rest are hidden as "???".
class RecipeBook {
 public:
  // Open at a tab, see tabForStation
  void open(int tab);
  void close() { is_open = false; }
  bool isOpen() const { return is_open; }

  // Tab for a crafting window, "crafting" uses the tier
  static int tabForStation(const std::string& window, int tier);

  // Mouse and keys while open
  void update();

  void draw(const GameState& state, const asw::Vec2i& ui_size) const;

 private:
  struct Layout {
    asw::Quadf book;
    asw::Quadf prev;
    asw::Quadf next;
    std::vector<asw::Quadf> tabs;
  };

  static Layout layout(const asw::Vec2i& ui_size);

  // Recipes on the current tab, in file order
  std::vector<const Recipe*> tabRecipes() const;
  int spreadCount() const;

  void turn(int delta);

  void drawRecipe(const GameState& state,
                  const Recipe& recipe,
                  const asw::Vec2f& at) const;

  bool is_open{false};
  int tab{0};

  // Two pages side by side
  int spread{0};
};

#endif  // SRC_UI_RECIPE_BOOK_H_
