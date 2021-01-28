# JimFarm

Our submission to ToJam 11.

A massive procedurally generated world in which you can set up a farm and collect resources.

## Getting started

### Windows (MSYS2)

#### Install Libraries (MSYS 2)

```bash
https://www.allegro.cc/files/?v=4.4
```

```bash
pacman -S mingw-w64-i686-nlohmann-json
```

#### Build

```bash
cmake -G "MSYS Makefiles" .
```

```bash
make
```

### Mac OS (Brew)

#### Install Libraries

```bash
https://github.com/msikma/liballeg.4.4.2-osx
```

```bash
brew tap nlohmann/json
brew install nlohmann-json
```

#### Build

```bash
cmake -G "Unix Makefiles" .
```

```bash
make
```

### Linux

#### Install Libraries

```bash
sudo apt-get install liballegro4-dev libloadpng4-dev liblogg4-dev
```

#### Build

```bash
cmake -G "Unix Makefiles" .
```

```bash
make
```
