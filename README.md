# JimFarm

Our submission to ToJam 11.

A massive procedurally generated world in which you can set up a farm and collect resources.

[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_JimFarm&metric=bugs)](https://sonarcloud.io/summary/new_code?id=AdsGames_JimFarm)

## Demo

[Web Demo](https://adsgames.github.io/JimFarm/)

## Setup

### Windows (MSYS2)

```bash
pacman -S mingw-w64-i686-gcc-libs mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_mixer mingw-w64-i686-SDL2_image mingw-w64-i686-SDL2_ttf
```

### Mac OS

```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

### Linux

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

### Build

```bash
mkdir build
cd build
cmake ..
make
```

### Build Emscripten

```bash
mkdir build
cd build
emcmake cmake ..
make
```
