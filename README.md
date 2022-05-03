# Balloons Buster

[![Linux Build](https://github.com/moonburnt/balloons_buster/actions/workflows/build_linux.yml/badge.svg)](https://github.com/moonburnt/balloons_buster/actions/workflows/build_linux.yml)
[![Windows Build](https://github.com/moonburnt/balloons_buster/actions/workflows/build_windows.yml/badge.svg)](https://github.com/moonburnt/balloons_buster/actions/workflows/build_windows.yml)

## Description

**Balloons Buster** - WIP balloon breaking game made with raylib + entt.

## Pre-requirements

- CMake 3.21+
- clang++ 13.0.1 (mac and linux only)

## Installation

```
git submodule update --init
git -C dependencies/engine submodule update --init
mkdir ./build
cmake . -B ./build
cmake --build ./build
```

Build files will be generated into ./build directory, and Game executable - into
./build/game (assets will be copied there too)
