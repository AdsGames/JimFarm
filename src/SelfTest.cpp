#include "SelfTest.h"

#include <asw/asw.h>

#include <string>

#include "Character.h"
#include "World.h"
#include "manager/InterfaceTypeManager.h"
#include "manager/RecipeManager.h"
#include "ui/Tooltip.h"

namespace {
int failures = 0;

void check(bool condition, const std::string& name) {
  if (condition) {
    asw::log::info("PASS {}", name);
  } else {
    asw::log::error("FAIL {}", name);
    failures++;
  }
}

std::shared_ptr<ItemStack> findStack(const std::string& id) {
  auto& inventory = World::playerInventory();
  for (int i = 0; i < inventory.getSize(); i++) {
    auto stack = inventory.getStack(i);
    if (stack->getItem() && stack->getItem()->getType().getId() == id) {
      return stack;
    }
  }
  return nullptr;
}

std::string idAt(World& world, const asw::Vec2i& pos, int layer) {
  auto tile = world.getMap().getTileAt(pos, layer);
  return tile ? tile->getType().getId() : "";
}
}  // namespace

int runSelfTest() {
  World::loadData();

  World world;
  Character jim;
  jim.loadData();

  world.newGame();
  jim.giveStarterItems();
  world.closeSummary();

  auto& state = world.getState();
  const auto home = state.home;
  const auto origin = home - asw::Vec2i(4, 7);

  // Starter farm
  check(idAt(world, origin + asw::Vec2i(3, 6), LAYER_FOREGROUND) ==
            "tile:barn",
        "barn placed");
  check(idAt(world, origin + asw::Vec2i(19, 6), LAYER_FOREGROUND) ==
            "tile:structure_part",
        "store footprint placed");
  check(world.getMap().isSolidAt(origin + asw::Vec2i(20, 6)),
        "store footprint is solid");

  // Farming loop: plant, water daily, harvest
  const auto plot = origin + asw::Vec2i(5, 10);
  const auto player_pos = (plot - asw::Vec2i(0, 1)) * TILE_SIZE;

  auto seeds = findStack("item:carrot_seed");
  check(seeds != nullptr, "has carrot seeds");
  const int seeds_before = seeds->getQuantity();
  world.interact(plot * TILE_SIZE, player_pos, *seeds);
  check(idAt(world, plot, LAYER_FOREGROUND) == "tile:carrot", "carrot planted");
  check(seeds->getQuantity() == seeds_before - 1, "seed consumed");

  auto can = findStack("item:watering_can");
  check(can != nullptr && can->getItem()->getMeta() > 0, "watering can full");

  for (int day = 0; day < 4; day++) {
    world.interact(plot * TILE_SIZE, player_pos, *can);
    check(idAt(world, plot, LAYER_MIDGROUND) == "tile:watered_soil",
          "soil watered day " + std::to_string(day + 1));
    world.endDay(false);
    world.closeSummary();
    can->getItem()->setMeta(10);
  }

  auto crop = world.getMap().getTileAt(plot, LAYER_FOREGROUND);
  check(crop && crop->getMeta() == MAX_TILE_META, "carrot ripe after 4 days");
  check(idAt(world, plot, LAYER_MIDGROUND) == "tile:plowed_soil" ||
            state.getWeather() == Weather::Rain,
        "soil dried overnight");

  ItemStack hand;
  world.interact(plot * TILE_SIZE, player_pos, hand);
  check(!world.getMap().getTileAt(plot, LAYER_FOREGROUND), "carrot harvested");
  auto dropped = world.getMap().getItemAt(plot);
  check(dropped && dropped->itemPtr->getType().getId() == "item:carrot",
        "carrot dropped");

  // Range limit
  world.interact((plot + asw::Vec2i(10, 10)) * TILE_SIZE, player_pos, *seeds);
  check(!world.getMap().getTileAt(plot + asw::Vec2i(10, 10), LAYER_FOREGROUND),
        "far tiles can not be used");

  // Eating
  state.hunger = 50.0F;
  auto berries = findStack("item:berry");
  world.consume(*berries);
  check(state.hunger > 50.0F, "eating restores hunger");

  // Cooking puts food in the bag, not under the fire
  const auto fire = origin + asw::Vec2i(14, 10);
  world.getMap().placeTile(std::make_shared<Tile>(
      "tile:campfire", fire * TILE_SIZE, LAYER_FOREGROUND, 200));
  World::playerInventory().addItem(std::make_shared<Item>("item:egg"), 1);
  auto egg = findStack("item:egg");
  const auto fire_player = (fire - asw::Vec2i(0, 1)) * TILE_SIZE;
  world.interact(fire * TILE_SIZE, fire_player, *egg);
  check(findStack("item:cooked_egg") != nullptr, "cooked egg goes to bag");
  check(!world.getMap().getItemAt(fire), "nothing dropped under the fire");

  // Drops never land on solid tiles
  world.dropItems("item:stick", fire);
  check(!world.getMap().getItemAt(fire), "drops avoid solid tiles");

  // Empty hand picks up items on a tile
  world.dropItems("item:stone", plot);
  world.interact(plot * TILE_SIZE, player_pos, hand);
  check(findStack("item:stone") != nullptr && !world.getMap().getItemAt(plot),
        "empty hand picks up items");

  // Messages expire
  auto& messages = world.getMessenger();
  messages.pushMessage("Hello");
  messages.update(3.0F);
  check(messages.messageCount() > 0, "messages stay a while");
  messages.update(10.0F);
  check(messages.messageCount() == 0, "messages expire");

  // Crafting
  auto crafting = InterfaceTypeManager::getInterfaceByName("crafting");
  std::vector<std::shared_ptr<ItemStack>> inputs = {
      std::make_shared<ItemStack>(std::make_shared<Item>("item:wood"), 1)};
  const auto* recipe = RecipeManager::match("crafting", inputs);
  check(recipe && recipe->output == "item:plank", "wood makes planks");
  RecipeManager::consume(*recipe, inputs);
  check(!inputs[0]->getItem(), "recipe consumes inputs");

  // Economy
  const int coins = state.getCoins();
  state.earn(25);
  check(state.getCoins() == coins + 25, "earning coins");
  check(!state.spend(state.getCoins() + 1), "can not overspend");

  // Survival: freezing hurts at night in winter-like cold
  const float ambient = world.ambientTemperature(home);
  check(ambient > -40.0F && ambient < 60.0F, "ambient temperature sane");

  // Night: wolves spawn and campfire gives light. Only chunks near the camera
  // tick, the player moves it in game.
  world.getCamera().pan(home * TILE_SIZE - world.getCamera().getSize() / 2);
  state.setMinutes(21 * 60);
  for (int i = 0; i < 20 * 60; i++) {
    world.update(1.0F / 60.0F, home * TILE_SIZE);
    if (!world.getCreatures().empty()) {
      break;
    }
  }
  check(!world.getCreatures().empty(), "wolves spawn at night");
  check(state.getWeather() != Weather::Sunny || state.weatherName() == "Clear",
        "no sun at night");
  // Lights are registered on the 50ms world tick
  for (int i = 0; i < 10; i++) {
    world.update(1.0F / 60.0F, home * TILE_SIZE);
  }
  check(world.nearLitCampfire(fire, 1), "campfire registers light");

  // Draw a night frame with the store open
  world.openShop();
  for (int frame = 0; frame < 3; frame++) {
    world.update(1.0F / 60.0F, home * TILE_SIZE);
    world.draw();
    world.getHud().draw(state);
    world.drawStatus(asw::Vec2i(480, 320));
    world.drawSummary(asw::Vec2i(480, 320));
    Tooltip::setItem(Item("item:tomato_seed"));
    Tooltip::draw(asw::Vec2i(480, 320));
    asw::display::present();
  }
  check(world.getHud().isOpen(), "store window opens");

  world.getHud().closeAll();

  // Save round trip
  const auto saved = world.toJson();
  World loaded;
  loaded.fromJson(saved);
  check(loaded.toJson()["map"] == saved["map"], "map survives save and load");
  check(loaded.getState().getCoins() == state.getCoins(),
        "state survives save and load");

  world.clearCreatures();

  asw::log::info("Self test finished with {} failure(s)", failures);
  return failures == 0 ? 0 : 1;
}
