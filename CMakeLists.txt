cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set project name and version
project(Game
    LANGUAGES CXX
    VERSION 0.1
)

add_executable(Game
    src/shared.cpp
    src/shared.hpp
    src/common.cpp
    src/common.hpp
    src/settings.cpp
    src/settings.hpp
    src/level.cpp
    src/level.hpp
    src/menus.cpp
    src/menus.hpp
    src/main.cpp
    dependencies/engine/core.cpp
    dependencies/engine/core.hpp
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

# Setup raylib
find_package(raylib REQUIRED)
target_link_libraries(Game ${raylib_LIBRARIES})
target_include_directories(Game PUBLIC ${raylib_INCLUDE_DIRS})

# Setup fmt
find_package(fmt REQUIRED)
target_link_libraries(Game fmt::fmt)
target_include_directories(Game PUBLIC ${fmt_INCLUDE_DIRS})

target_include_directories(Game
    PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/src"
)

target_compile_features(Game
    PRIVATE
    cxx_std_17
)

# Set compile flags
target_compile_options(Game
    PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -ferror-limit=5
)
