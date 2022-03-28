#pragma once

#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "entt/entity/registry.hpp"
#include "raylib.h"
#include <string>

class Level : public Scene {
private:
    // Level's registry that will hold our entities.
    entt::registry registry;

    Vector2 room_size;
    // Max enemies amount
    int max_enemies;
    // Enemies currently on screen. If < max_enemies, new enemies will spawn
    int enemies_left;
    int score;
    // Player lifes left
    int lifes;

    Label score_counter;
    Label life_counter;

    // Balls spawn cooldown
    Timer spawn_timer;

    // Component handlers
    void process_collisions(Vector2 mouse_pos);
    void move_balls(float dt);
    void draw_balls();
    void spawn_balls(int amount);

    void damage_player();
    void kill_enemy(entt::entity entity);

public:
    Level(Vector2 room_size);
    Level();

    void update(float dt) override;
    void draw() override;
};
