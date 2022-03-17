#include "core.hpp"
#include <cstdlib>
#include <iostream>

#include "raylib.h"

// Scene stuff
Scene::Scene(Color _bg_color)
    : bg_color(_bg_color) {
}

Scene::Scene()
    : bg_color(Color{245, 245, 245, 255}) {
}

// Scene manager

// There are two ways to work with scenes: to add scene manually each time,
// initializing it from zero and clearing up other scenes from memory.
// Or to keep all scenes initialized in some storage. For now, we are going for
// the first one, but this behavior may change in future.
void SceneManager::set_current_scene(Scene* scene) {
    if (current_scene != nullptr) {
        delete current_scene;
    };

    current_scene = scene;
    // active = true;
}

bool SceneManager::is_active() {
    return !WindowShouldClose() && active;
}

void SceneManager::run_update_loop() {
    while (is_active()) {
        // Because we don't really need double precision there
        float dt = static_cast<float>(GetFrameTime());
        current_scene->update(dt);

        BeginDrawing();
        ClearBackground(current_scene->bg_color);
        current_scene->draw();

        // // Maybe I should store it somewhere?
        // if (SettingsManager::manager.get_show_fps()) {
        //     DrawText(TextFormat("FPS: %02i", GetFPS()), 1200, 4, 20, BLACK);
        // }
        EndDrawing();
    }

    std::cout << "Attempting to shutdown the game.\n";
    CloseWindow();
}

// Default SceneManager's constructor is all way down, coz TitleScreen is in its
// body. But don't worry - even if instantiation is declared above, nothing bad
// will happen - this one will get triggered correctly
SceneManager::SceneManager() {
    // Setting current_scene to null, to avoid segfault below.
    current_scene = nullptr;

    // active = false;
    active = true;
}

// GameWindow stuff
GameWindow::GameWindow() {
    initialized = false;
}

void GameWindow::init(int x, int y, std::string title, int fps) {
    InitWindow(x, y, title.c_str());
    // Setting window's framerate
    SetTargetFPS(fps);
    // Disable ability to close the window by pressing esc
    SetExitKey(KEY_NULL);

    // Initialize audio device. This needs to be done before loading sounds.
    InitAudioDevice();

    initialized = true;
}

void GameWindow::init(int x, int y, std::string title) {
    init(x, y, title, 60);
}

void GameWindow::init(int x, int y) {
    init(x, y, "Game Window");
}

void GameWindow::init() {
    init(1280, 720);
}

void GameWindow::run() {
    if (!initialized) {
        std::cout << "Attempting to run unitialized GameWindow. Did you forget "
                     "GameWindow.init()?\n";
        abort();
    };

    sc_mgr.run_update_loop();
}
