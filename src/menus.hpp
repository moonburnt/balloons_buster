#pragma once

#include "engine/core.hpp"
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
