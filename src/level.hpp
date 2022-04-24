#pragma once

#include "box2d/b2_world.h"
#include "components.hpp"
#include "engine/core.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "box2d/box2d.h"
#include "entt/entity/registry.hpp"
#include "event_screens.hpp"
#include "raylib.h"
#include <optional>
#include <string>
#include <tuple>

class Level : public Scene {
private:
    SceneManager* parent;

    // Specifies if Level must be closed
    bool must_close = false;

    // Level's registry that will hold our entities.
    entt::registry registry;

    Vector2 room_size;

    b2World world;

    // Collision stuff
    float accumulator = 0;
    float phys_time = 1 / 60.0f;

    Camera2D camera;

    // Max enemies amount
    int max_enemies;
    // Enemies currently on screen. If < max_enemies, new enemies will spawn
    int enemies_left;
    int enemies_killed;
    int score;
    // Player lifes left
    int lifes;

    Label score_counter;
    Label life_counter;
    Label kill_counter;

    // Balls spawn cooldown
    Timer spawn_timer;

    bool is_gameover = false;
    GameoverScreen gameover_screen;

    bool is_paused = false;
    PauseScreen pause_screen;
    Button pause_button;

    // Collision tree shenanigans
    void update_collisions_tree(float dt);
    void process_mouse_collisions(Vector2 mouse_pos);

    // Purge entity with provided collision component
    // Should only be casted after tree has been updated.
    void remove_collision_entity(CollisionComponentBase* component);

    void spawn_walls();
    void draw_walls();

    void spawn_balls(int amount);
    void draw_balls();

    void damage_player();
    void kill_enemy(entt::entity entity);
    void resume();
    void exit_to_menu();

public:
    Level(SceneManager* p, Vector2 room_size);
    Level(SceneManager* p);

    ~Level();

    void update(float dt) override;
    void draw() override;
};
