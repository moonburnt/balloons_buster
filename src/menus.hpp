#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "raylib.h"

class TitleScreen : public Scene {
private:
    SceneManager* parent;
    Timer* timer;
    std::string greeter_msg;
    Vector2 greeter_pos;

public:
    TitleScreen(SceneManager* p);

    void update(float dt) override;
    void draw() override;
};

class MainMenu : public Scene {
private:
    enum MM_BUTTONS {
        MM_NEWGAME,
        MM_SETTINGS,
        MM_EXIT,
        MM_CONTINUE
    };

    SceneManager* parent;
    ButtonStorage buttons;

    void call_exit();
    void new_game();
    void load_game();
    void open_settings();

public:
    MainMenu(SceneManager* p);

    void update(float) override;
    void draw() override;
};
