#pragma once

#include "engine/core.hpp"
#include "entt/entity/registry.hpp"
#include "raylib.h"
#include <string>

class Level : public Scene {
private:
    // Level's registry that will hold our entities.
    entt::registry registry;

    Vector2 room_size;

public:
    Level(Vector2 room_size);
    Level();

    void update(float dt) override;
    void draw() override;
};
