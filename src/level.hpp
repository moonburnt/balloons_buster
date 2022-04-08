#pragma once

#include "engine/core.hpp"
#include "engine/quadtree.hpp"
#include "engine/ui.hpp"
#include "engine/utility.hpp"
#include "entt/entity/registry.hpp"
#include "event_screens.hpp"
#include "raylib.h"
#include <optional>
#include <string>
#include <tuple>

class Level : public Scene {
private:
    SceneManager* parent;

    // Level's registry that will hold our entities.
    entt::registry registry;

    Vector2 room_size;

    // QuadTree<std::tuple<entt::entity, Vector2>> tree;
    QuadTree<std::tuple<entt::entity, Rectangle>> tree;

    Vector2 pointer_size;
    Rectangle pointer_rect;

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

    std::optional<GameoverScreen> gameover_screen;

    // Collision tree shenanigans
    void update_collisions_tree();
    void process_ball_collisions();
    // This should probably be the last one since it destroys objects on tree,
    // and there is no mechanism to remove these directly from it right now.
    void process_mouse_collisions(Vector2 mouse_pos);

    // Component handlers
    void move_balls(float dt);
    void draw_balls();
    void spawn_balls(int amount);

    void damage_player();
    void kill_enemy(entt::entity entity);
    void exit_to_menu();

public:
    Level(SceneManager* p, Vector2 room_size);
    Level(SceneManager* p);

    void update(float dt) override;
    void draw() override;
};
