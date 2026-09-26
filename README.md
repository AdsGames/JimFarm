# JimFarm

Our submission to ToJam 11.

A massive procedurally generated world in which you can set up a farm and collect resources.

[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=bugs)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)

## How to play

Earn $1000 by the end of Spring. Till soil, plant seeds, water them every day,
then sell the harvest at the store. Sleep in the barn to end the day. The game
saves each morning.

Stay alive: eat (hold right click), drink at water or the well, and stay warm.
Wolves hunt you and your chickens at night. Lit campfires keep them away and
warm you. Fences keep them out.

Explore: each biome has its own finds. Mushrooms grow in cold forests, herbs
in dry grassland, cactus fruit in the desert, and reeds, clams and clay on the
shore. Rock hides copper ore, and iron ore where it is cold. Hunt deer and
rabbits by day for meat and hide.

Build: craft a workbench by hand, then a pickaxe and a kiln at the workbench.
The kiln makes glass, bricks and ingots. Ingots upgrade your tools, and a stone
workbench makes iron tools. Walls, windows and a door make a warm room that
wolves can not enter. A bed sets where you wake up. Sprinklers water crops,
scarecrows keep crows away and hoppers collect eggs. New recipes appear in the
recipe book (R) as you pick up new things.

| Key                     | Action                         |
| ----------------------- | ------------------------------ |
| WASD / arrows           | Move                           |
| Left click / Space      | Use held item (tools, weapons) |
| Right click / C         | Interact: open, harvest, pick  |
| Hold right click / C    | Eat or drink held food         |
| 1-8, Z / X, mouse wheel | Select hotbar slot             |
| F                       | Drop item                      |
| E / Q / G               | Bag / crafting / furnace       |
| R                       | Recipe book (1-5 tabs, A D)    |
| H                       | Controls and recipes           |
| + / -                   | Zoom                           |

In windows (Minecraft style):

| Input                          | Action                                   |
| ------------------------------ | ---------------------------------------- |
| Click                          | Take a stack, place it, or swap stacks   |
| Right click                    | Take half, or place one                  |
| Drag / right drag              | Spread evenly / one in each slot         |
| Shift click                    | Move to the other window, or craft all   |
| Double click                   | Collect all of the held item             |
| 1-8 over a slot                | Swap with that hotbar slot               |
| F / Ctrl F over a slot         | Drop one / drop the stack                |
| Click outside with held items  | Throw them (right click throws one)      |
| Esc                     | Close windows, pause menu      |

## Modding

Game content is data in `assets/data/`:

- `tiles.json`: tiles and their `behaviours` (crop, animal, campfire, shop, bed,
  station, breakable, sprinkler, scarecrow, hopper, ...). `encloses` marks walls
  and doors that make rooms
- `items.json`: prices, food, and `actions`, the rules for what an item does to
  a tile. `power` makes a tool break trees and rock faster, `actions_from`
  reuses another item's rules
- `recipes.json`: crafting, furnace and kiln recipes. `tier` 1 needs a
  workbench, 2 a stone workbench
- `interfaces.json`: windows, including the store stock

Placeholder art is in `assets/images/placeholders.png`. Run
`python3 gen_placeholders.py` to make it again.

## Demo

[Web Demo](https://adsgames.github.io/JimFarm/)

## Setup

### CMake

```bash
cmake --preset debug
cmake --build --preset debug
```

### Self test

Plays core systems (farming, crafting, wolves, save and load) with no input:

```bash
cd build/debug/target && ./JimFarm --selftest
```

### Build Emscripten

```bash
emcmake cmake --preset debug
cmake --build --preset debug
```
