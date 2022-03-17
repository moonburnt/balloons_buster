#pragma once

#include "engine/engine.hpp"
#include "entt/entity/registry.hpp"
#include "raylib.h"
#include <string>

class Level : public Scene {
private:
    // Level's registry that will hold our entities.
    entt::registry registry;

    int level_x;
    int level_y;

public:
    Level(int x, int y);
    Level();

    void update(float dt) override;
    void draw() override;
};
