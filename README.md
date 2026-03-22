# JimFarm

Our submission to ToJam 11.

A massive procedurally generated world in which you can set up a farm and collect resources.

[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=bugs)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)

## How to play

Earn $1000 by the end of Spring. Till soil, plant seeds, water them every day,
then sell the harvest at the store. Sleep in the barn to end the day. The game
saves each morning.

Stay alive: eat (C or right click), drink at water or the well, and stay warm.
Wolves hunt you and your chickens at night. Lit campfires keep them away and
warm you. Fences keep them out.

| Key                     | Action                         |
| ----------------------- | ------------------------------ |
| WASD / arrows           | Move                           |
| Click / Space           | Use held item (empty hand too) |
| C / right click         | Eat or drink held item         |
| 1-8, Z / X, mouse wheel | Select hotbar slot             |
| F                       | Drop item                      |
| E / Q / G               | Bag / crafting / furnace       |
| H                       | Controls and recipes           |
| + / -                   | Zoom                           |
| Esc                     | Close windows, pause menu      |

## Modding

Game content is data in `assets/data/`:

- `tiles.json`: tiles and their `behaviours` (crop, animal, campfire, shop, bed, ...)
- `items.json`: prices, food, and `actions`, the rules for what an item does to a tile
- `recipes.json`: crafting and furnace recipes
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
