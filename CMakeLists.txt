cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set project name and version
project(Game
    LANGUAGES CXX
    VERSION 0.1
)

add_executable(Game
    src/main.cpp
    dependencies/engine/engine.cpp
    dependencies/engine/engine.hpp
)

find_library(RAYLIB NAMES raylib REQUIRED)
find_library(FMT NAMES fmt REQUIRED)

target_link_libraries(Game
    raylib
    fmt
)

target_include_directories(Game
    PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/src"
)

target_compile_features(Game
    PRIVATE
    cxx_std_17
)

target_compile_options(Game
    PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override
)
