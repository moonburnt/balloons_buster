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
#    add_definitions(-DDEBUG -O0 -g)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_executable(Game
    src/shared.cpp
    src/shared.hpp
    src/event_screens.cpp
    src/event_screens.hpp
    src/common.cpp
    src/common.hpp
    src/components.cpp
    src/components.hpp
    src/level.cpp
    src/level.hpp
    src/menus.cpp
    src/menus.hpp
    src/main.cpp
)

target_include_directories(Game
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

target_compile_options(Game
    PUBLIC
    -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers
)

# Setup engine
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/engine")
target_link_libraries(Game engine)
target_include_directories(Game PUBLIC ${engine_INCLUDE_DIRS})

# Setup entt
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/entt")
target_link_libraries(Game EnTT)
target_include_directories(Game PUBLIC ${EnTT_INCLUDE_DIRS})

# Setup box2d
# Disable building of some features. Because testbed will mess with raylib's glfw
# target and unit tests aren't necessary for us
set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_UNIT_TESTS OFF)
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/box2d")
target_link_libraries(Game box2d)
target_include_directories(Game PUBLIC ${box2d_INCLUDE_DIRS})


add_custom_target(compile_commands
  WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
  BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
  COMMAND cp -f ./compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
  VERBATIM)

add_custom_target(format
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND find src \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
  VERBATIM)
