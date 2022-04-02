cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set project name and version
project(Game
    LANGUAGES CXX
    VERSION 0.1
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("---- RELEASE MODE ----")
    add_definitions(-O2)
else()
    message("---- DEBUG MODE ----")
    add_definitions(-DDEBUG -O0 -g)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_definitions(-Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers)

add_executable(Game
    src/shared.cpp
    src/shared.hpp
    src/event_screens.cpp
    src/event_screens.hpp
    src/common.cpp
    src/common.hpp
    src/level.cpp
    src/level.hpp
    src/menus.cpp
    src/menus.hpp
    src/main.cpp
    dependencies/engine/core.cpp
    dependencies/engine/core.hpp
    dependencies/engine/quadtree.hpp
    dependencies/engine/settings.cpp
    dependencies/engine/settings.hpp
    dependencies/engine/sound.cpp
    dependencies/engine/sound.hpp
    dependencies/engine/sprite.cpp
    dependencies/engine/sprite.hpp
    dependencies/engine/ui.cpp
    dependencies/engine/ui.hpp
    dependencies/engine/utility.cpp
    dependencies/engine/utility.hpp
    dependencies/engine/storage.cpp
    dependencies/engine/storage.hpp
)

target_include_directories(Game
    PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/src"
)

# Setup raylib
find_package(raylib REQUIRED)
target_link_libraries(Game ${raylib_LIBRARIES})
target_include_directories(Game PUBLIC ${raylib_INCLUDE_DIRS})

# Setup fmt
find_package(fmt REQUIRED)
target_link_libraries(Game fmt::fmt)
target_include_directories(Game PUBLIC ${fmt_INCLUDE_DIRS})

# Setup spdlog
find_package(spdlog REQUIRED)
target_link_libraries(Game spdlog::spdlog_header_only)
target_include_directories(Game PUBLIC ${spdlog_INCLUDE_DIRS})

