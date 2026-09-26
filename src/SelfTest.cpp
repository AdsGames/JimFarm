#include "SelfTest.h"

#include <asw/asw.h>

#include <cmath>
#include <string>

#include "Character.h"
#include "World.h"
#include "manager/InterfaceTypeManager.h"
#include "manager/RecipeManager.h"
#include "manager/TileTypeManager.h"
#include "ui/Tooltip.h"
#include "ui/UiSlot.h"

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

  // Recipe tiers: a pickaxe needs a workbench
  std::vector<std::shared_ptr<ItemStack>> pick_inputs = {
      std::make_shared<ItemStack>(std::make_shared<Item>("item:stick"), 2),
      std::make_shared<ItemStack>(std::make_shared<Item>("item:stone"), 3),
      std::make_shared<ItemStack>(std::make_shared<Item>("item:plank"), 1)};
  check(!RecipeManager::match("crafting", pick_inputs, TIER_HAND),
        "pickaxe not made by hand");
  check(RecipeManager::needsTier("crafting", pick_inputs, TIER_HAND) != nullptr,
        "pickaxe shows it needs a station");
  const auto* pick = RecipeManager::match("crafting", pick_inputs,
                                          TIER_WORKBENCH);
  check(pick && pick->output == "item:pickaxe", "workbench makes a pickaxe");

  std::vector<std::shared_ptr<ItemStack>> gate_inputs = {
      std::make_shared<ItemStack>(std::make_shared<Item>("item:plank"), 2),
      std::make_shared<ItemStack>(std::make_shared<Item>("item:stick"), 2)};
  const auto* gate = RecipeManager::match("crafting", gate_inputs);
  check(gate && gate->output == "item:gate", "gate can be crafted");

  // Helpers for the building tests
  auto place = [&](const std::string& id, const asw::Vec2i& at,
                   int layer = LAYER_FOREGROUND) {
    if (auto old = world.getMap().getTileAt(at, layer)) {
      world.getMap().removeTile(old);
    }
    world.getMap().placeTile(
        std::make_shared<Tile>(id, at * TILE_SIZE, layer));
  };
  auto give = [&](const std::string& id) {
    World::playerInventory().addItem(std::make_shared<Item>(id), 1);
    return findStack(id);
  };
  auto use = [&](const asw::Vec2i& at, ItemStack& stack) {
    state.energy = MAX_STAT;
    world.interact(at * TILE_SIZE, (at - asw::Vec2i(0, 1)) * TILE_SIZE, stack);
  };
  // Picks up everything around at, true if id was there
  auto dropsNear = [&](const asw::Vec2i& at, const std::string& id) {
    const int before = World::playerInventory().count(id);
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        world.pickUpItems(at + asw::Vec2i(dx, dy));
      }
    }
    return World::playerInventory().count(id) > before;
  };
  auto tick = [&]() { world.update(0.0F, home * TILE_SIZE); };

  // Rooms: a 5x5 wall ring with a door is indoors and warmer
  const auto room = origin + asw::Vec2i(1, 12);
  for (int i = 0; i < 5; i++) {
    place("tile:wood_wall", room + asw::Vec2i(i, 0));
    place("tile:wood_wall", room + asw::Vec2i(i, 4));
    place("tile:wood_wall", room + asw::Vec2i(0, i));
    place("tile:wood_wall", room + asw::Vec2i(4, i));
  }
  place("tile:door", room + asw::Vec2i(2, 4));
  tick();
  const auto inside = room + asw::Vec2i(2, 2);
  check(world.isIndoors(inside), "walled room with a door is indoors");
  check(!world.isIndoors(room + asw::Vec2i(2, 6)), "outside is not indoors");
  check(world.ambientTemperature(inside) >
            world.ambientTemperature(room + asw::Vec2i(2, 6)),
        "rooms are warmer");

  // Bed sets where you wake up
  const auto bed = inside + asw::Vec2i(0, -1);
  place("tile:bed", bed);
  const auto old_home = state.home;
  use(bed, hand);
  check(state.sleep_requested && state.home != old_home,
        "bed sets the wake up spot");
  state.sleep_requested = false;
  state.home = old_home;

  // Opening a wall lets the cold in
  world.getMap().removeTile(
      world.getMap().getTileAt(room + asw::Vec2i(4, 2), LAYER_FOREGROUND));
  tick();
  check(!world.isIndoors(inside), "open wall is not a room");

  // Workbench opens crafting at tier 1, an axe picks it up
  const auto bench = origin + asw::Vec2i(10, 14);
  place("tile:workbench", bench);
  use(bench, hand);
  check(world.getHud().isOpen("crafting") &&
            state.crafting_tier == TIER_WORKBENCH,
        "workbench opens crafting");
  world.getHud().closeAll();
  tick();
  check(state.crafting_tier == TIER_HAND, "closing resets the tier");
  auto axe = findStack("item:axe");
  use(bench, *axe);
  check(!world.getMap().getTileAt(bench, LAYER_FOREGROUND) &&
            dropsNear(bench, "item:workbench"),
        "axe picks up the workbench");

  // Ore needs the right pickaxe
  const auto rock = origin + asw::Vec2i(12, 14);
  place("tile:iron_ore", rock);
  auto pickaxe = give("item:pickaxe");
  use(rock, *pickaxe);
  check(world.getMap().getTileAt(rock, LAYER_FOREGROUND) != nullptr,
        "pickaxe can not break iron ore");
  place("tile:copper_ore", rock);
  for (int i = 0; i < 4; i++) {
    use(rock, *pickaxe);
  }
  check(!world.getMap().getTileAt(rock, LAYER_FOREGROUND) &&
            dropsNear(rock, "item:copper_ore"),
        "pickaxe mines copper ore");

  // Better axes fell trees faster
  const auto tree = origin + asw::Vec2i(12, 16);
  place("tile:tree", tree);
  auto iron_axe = give("item:iron_axe");
  use(tree, *iron_axe);
  check(idAt(world, tree, LAYER_FOREGROUND) == "tile:stump",
        "iron axe fells a tree in one hit");

  // Forage by hand
  const auto shroom = origin + asw::Vec2i(14, 14);
  place("tile:mushroom", shroom);
  use(shroom, hand);
  check(dropsNear(shroom, "item:mushroom"), "mushrooms can be picked");

  // Hunting a deer drops meat and hide
  const auto deer_at = origin + asw::Vec2i(10, 16);
  auto deer =
      std::make_shared<Creature>(deer_at * TILE_SIZE, CreatureKind::Deer);
  world.getCreatures().push_back(deer);
  for (int i = 0; i < 3 && !deer->isGone(); i++) {
    use(deer->getTile(), *iron_axe);
  }
  const int hides = World::playerInventory().count("item:hide");
  check(deer->isGone() && dropsNear(deer->getTile(), "item:meat") &&
            World::playerInventory().count("item:hide") > hides,
        "deer drops meat and hide");
  world.clearCreatures();

  // Startled grazers run to a spot away from the player, then calm down
  {
    const auto field = origin + asw::Vec2i(12, 3);
    const auto player_px = (field - asw::Vec2i(2, 0)) * TILE_SIZE;
    auto rabbit =
        std::make_shared<Creature>(field * TILE_SIZE, CreatureKind::Rabbit);
    auto distance = [&]() {
      const auto offset = rabbit->getPosition() - player_px;
      return std::sqrt(static_cast<float>(offset.x * offset.x +
                                          offset.y * offset.y)) /
             TILE_SIZE;
    };

    for (int i = 0; i < 60 * 8; i++) {
      rabbit->update(world, player_px, 1.0F / 60.0F);
    }
    check(!rabbit->isGone() && distance() >= 9.5F, "rabbit flees far enough");

    const auto calm_at = rabbit->getPosition();
    for (int i = 0; i < 60 * 2; i++) {
      rabbit->update(world, player_px, 1.0F / 60.0F);
    }
    const auto moved = rabbit->getPosition() - calm_at;
    check(std::abs(moved.x) + std::abs(moved.y) < 60,
          "rabbit calms down once far away");
  }

  // Sprinklers water soil each morning, hoppers catch eggs
  const auto sprinkler = origin + asw::Vec2i(7, 13);
  place("tile:sprinkler", sprinkler);
  place("tile:plowed_soil", sprinkler + asw::Vec2i(1, 0), LAYER_MIDGROUND);
  const auto coop = origin + asw::Vec2i(18, 13);
  place("tile:hopper", coop + asw::Vec2i(1, 0));
  place("tile:chicken", coop);
  auto hay = give("item:hay");
  use(coop, *hay);
  world.endDay(false);
  world.closeSummary();
  check(idAt(world, sprinkler + asw::Vec2i(1, 0), LAYER_MIDGROUND) ==
            "tile:watered_soil",
        "sprinkler waters soil");
  auto hopper = world.getMap().getTileAt(coop + asw::Vec2i(1, 0),
                                         LAYER_FOREGROUND);
  check(hopper && hopper->getMeta() == 1, "hopper collects the egg");

  // Recipe discovery
  tick();
  check(state.known_items.contains("item:hoe"), "held items are known");
  give("item:clay");
  tick();
  check(state.known_items.contains("item:clay"), "new items are learned");

  // Inventory shortcuts: shift click moves stacks and crafts all
  {
    auto& hud = world.getHud();
    auto& bag = World::playerInventory();
    hud.closeAll();
    hud.open("inventory");

    auto* bag_ui = hud.window("inventory");
    auto hotbar = bag.getStack(7);
    hotbar->setItem(std::make_shared<Item>("item:stone"), 3);
    hud.quickMove(*bag_ui, SlotType::Input, *hotbar, state);
    check(!hotbar->getItem() && bag.count("item:stone") >= 3,
          "shift click moves hotbar to bag rows");

    hud.open("crafting");
    auto* crafting_ui = hud.window("crafting");
    auto wood = bag.getStack(6);
    wood->setItem(std::make_shared<Item>("item:wood"), 3);
    hud.quickMove(*bag_ui, SlotType::Input, *wood, state);
    const auto inputs = crafting_ui->stacksOfType(SlotType::Input);
    check(!wood->getItem() && inputs[0]->getItem() &&
              inputs[0]->getQuantity() == 3,
          "shift click moves bag to crafting");

    const int planks = bag.count("item:plank");
    const auto output = crafting_ui->stacksOfType(SlotType::Output).front();
    hud.quickMove(*crafting_ui, SlotType::Output, *output, state);
    check(bag.count("item:plank") == planks + 12 && !inputs[0]->getItem(),
          "shift click on output crafts all");

    inputs[1]->setItem(std::make_shared<Item>("item:clay"), 2);
    const int clay = bag.count("item:clay");
    hud.quickMove(*crafting_ui, SlotType::Input, *inputs[1], state);
    check(!inputs[1]->getItem() && bag.count("item:clay") == clay + 2,
          "shift click moves crafting back to bag");
    hud.closeAll();
  }

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
