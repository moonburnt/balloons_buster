#pragma once

#include "raylib.h"
#include <string>

// Scene is an abstract class that can't be instantiated directly, but can be
// subclassed. This is how we do ABC interfaces in c++
// In this case its located in header, coz SceneManager needs it
class Scene {
public:
    // Thats how we define abstract functions
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // This is a scene's destructor. Which can be overriden, but not necessary.
    virtual ~Scene() = default;

    Color bg_color;

    Scene();
    Scene(Color bg_color);
};

class SceneManager {
    // We are using pointer to Scene, to make it work with Scene's children

private:
    Scene* current_scene;

public:
    SceneManager();
    void set_current_scene(Scene* scene);
    void run_update_loop();
    bool active;
    bool is_active();
};

class GameWindow {
protected:
    bool initialized;

public:
    SceneManager sc_mgr;

    GameWindow();

    // Initialize game window
    void init(int x, int y, std::string title, int fps);
    void init(int x, int y, std::string title);
    void init(int x, int y);
    void init();

    // Run scene manager and other stuff
    void run();
};
