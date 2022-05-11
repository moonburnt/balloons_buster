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

class App;

class Wind {
private:
    b2World* world;

    float min_timer_length;
    float max_timer_length;
    float min_power;
    float max_power;

    Timer timer;

    void blow(b2Vec2 wind);

public:
    Wind(
        b2World* world,
        float min_timer_length,
        float max_timer_length,
        float min_power,
        float max_power);

    void update(float dt);
};

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
    Button* pause_button;
    App* app;

    Wind wind;

    // Collision tree shenanigans
    void update_collisions_tree(float dt);
    void process_mouse_collisions(Vector2 mouse_pos);

    void spawn_walls();
    void draw_walls();

    void spawn_balls(int amount);
    void draw_balls();

    void damage_player();
    void resume();
    void exit_to_menu();
    void cleanup_physics(entt::registry& reg, entt::entity e);

    void validate_physics();

public:
    Level(App* app, SceneManager* p, Vector2 room_size);
    Level(App* app, SceneManager* p);
    ~Level();

    void update(float dt) override;
    void draw() override;
};
