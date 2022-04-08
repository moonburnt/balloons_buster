#include "level.hpp"
#include "common.hpp"
#include "event_screens.hpp"
#include "menus.hpp"
// For vector normalizaion
#include "entt/entity/fwd.hpp"
#include "raymath.h"
// For basic formatting
#include <fmt/core.h>
// For logging
#include "shared.hpp"
#include "spdlog/spdlog.h"
// To log entity
#include <cinttypes>
// For rand()
#include <cstdlib>
#include <raylib.h>
#include <unistd.h>

// Our components.
// Position component will be just Vector2 - no need to specify it, but we can
// typedef an alias, to make it easier to understand.
// TODO: maybe revert it back to be part of BallComponent?
typedef Vector2 PosComp;

// Rectangle around objects for faster collisions. Should be bigger than
// actual collision component, if its not the one.
typedef Rectangle RectComp;

// BallComponent. Require PosComp to work
struct BallComponent {
    Color color;
    float radius;
};

// MovementCompoment. Require PosComp to work
struct MovementComponent {
    Vector2 direction;
    float speed;
};

// bool check_collision(std::tuple<entt::entity, Vector2> obj, Rectangle rect) {
//     return CheckCollisionPointRec(std::get<1>(obj), rect);
// }
bool check_collision(std::tuple<entt::entity, Rectangle> obj, Rectangle rect) {
    return CheckCollisionRecs(std::get<1>(obj), rect);
}

void Level::update_collisions_tree() {
    // Clearing up results from previous frame
    tree.clear();

    // Filling it up with new data
    auto this_view = registry.view<RectComp>();

    for (auto entity : this_view) {
        tree.insert(
            std::tuple<entt::entity, Rectangle>(entity, this_view.get<RectComp>(entity)));
    }
}

void Level::process_ball_collisions() {
    auto this_view = registry.view<RectComp, BallComponent, PosComp, MovementComponent>();

    for (auto entity : this_view) {
        auto possible_cols = tree.query_range(this_view.get<RectComp>(entity));
        spdlog::debug(
            "Found {} possible collisions for {}",
            possible_cols.size(), static_cast<std::uint32_t>(entity));

        if (possible_cols.size() > 0) {
            BallComponent& ball = this_view.get<BallComponent>(entity);
            PosComp& pos = this_view.get<PosComp>(entity);

            for (auto col : possible_cols) {
                entt::entity col_target = std::get<0>(col);

                // I have a gut feeling this may explode under some conditions...
                if (col_target != entity && CheckCollisionCircles(pos, ball.radius, this_view.get<PosComp>(col_target), this_view.get<BallComponent>(col_target).radius)) {
                    // This can be done better and more realistically, affecting
                    // the speed of both collider and collideable. But for now
                    // it will do, I guess?
                    MovementComponent& mov = this_view.get<MovementComponent>(col_target);
                    mov.direction.x = -mov.direction.x;
                    registry.replace<MovementComponent>(col_target, mov);
                }
            }
        }
    }
}

void Level::process_mouse_collisions(Vector2 mouse_pos) {
    // Update mouse pointer's rect. May move it somewhere else later
    pointer_rect.x = mouse_pos.x - pointer_size.x / 2;
    pointer_rect.y = mouse_pos.y - pointer_size.y / 2;

    // This may go wrong if PosComp will ever be attached to things that should
    // not collide with player's pointer
    auto this_view = registry.view<PosComp, BallComponent>();

    // This kinda works. But also kinda not - some balloons for whatever reasons
    // do not get detected as potential collisions by query.
    // I have no idea why is it bugged like that. TODO
    auto results = tree.query_range(pointer_rect);
    spdlog::debug("Found {} potential collisions, processing", results.size());

    // for (auto ent_pair : tree.query_range(pointer_rect)) {
    for (auto ent_pair : results) {
        auto entity = std::get<0>(ent_pair);

        PosComp& pos = this_view.get<PosComp>(entity);
        BallComponent& ball = this_view.get<BallComponent>(entity);

        if (CheckCollisionPointCircle(mouse_pos, pos, ball.radius)) {
            // TODO: add death animation, schedule destroying
            kill_enemy(entity);
            // TODO: "RewardComp" with different score values depending on type
            score += 10;
            score_counter.set_text(fmt::format("Score: {}", score));
            enemies_killed++;
            kill_counter.set_text(fmt::format("Balloons Popped: {}", enemies_killed));
            // For now, its only possible to pop one balloon at once
            return;
        };
    }
}

void Level::move_balls(float dt) {
    // auto this_view = registry.view<PosComp, MovementComponent>();
    auto this_view = registry.view<RectComp, PosComp, MovementComponent>();

    // There are many ways to iterate through components in EnTT.
    // For the sake of simplicity, we will use a simple for loop there.
    for (auto entity : this_view) {
        MovementComponent& movement = this_view.get<MovementComponent>(entity);
        PosComp& pos = this_view.get<PosComp>(entity);

        pos.x += movement.direction.x * (movement.speed * dt);
        pos.y += movement.direction.y * (movement.speed * dt);

        // I hope this won't explode
        if (pos.y < 0) {
            damage_player();
            kill_enemy(entity);
            continue;
        }

        // Ensuring balls will never leave screen's area
        if (pos.x < 0 || pos.x > room_size.x) {
            movement.direction.x = -movement.direction.x;
            // In case some changes has been made to an entity - we would need
            // to replace the stored component.
            registry.replace<MovementComponent>(entity, movement);
        }

        RectComp& rect = this_view.get<RectComp>(entity);

        rect.x = pos.x - rect.width / 2.0f;
        rect.y = pos.y - rect.height / 2.0f;

        registry.replace<PosComp>(entity, pos);
        registry.replace<RectComp>(entity, rect);
    }
}

void Level::draw_balls() {
    // auto this_view = registry.view<PosComp, BallComponent>();

    // this_view.each([](const auto& pos, const auto& ball) {
    //     DrawCircleV(pos, ball.radius, ball.color);
    // });

    auto this_view = registry.view<RectComp, PosComp, BallComponent>();

    this_view.each([](const auto& rect, const auto& pos, const auto& ball) {
        DrawRectangleRec(rect, BLACK);
        DrawCircleV(pos, ball.radius, ball.color);
    });

    DrawRectangleRec(pointer_rect, RED);
}

void Level::spawn_balls(int amount) {
    spdlog::debug("Attempting to spawn {} enemies", amount);
    for (int i = 0; i < amount; i++) {
        // First we need to initialize an empty entity with no components.
        // This will make registry assign an unique entity id to it and return it.
        entt::entity ball = registry.create();

        // Now lets initialize and attach all required components to our entity id.
        float x = static_cast<float>(std::rand() % static_cast<int>(room_size.x));
        float y = room_size.y + 50.0f;
        float size = static_cast<float>(std::rand() % 50) + 10.0f;

        registry.emplace<PosComp>(ball, PosComp{x, y});
        // registry.emplace<BallComponent>(ball, get_rand_color(), size);
        registry.emplace<BallComponent>(ball, BLUE, size);
        registry.emplace<MovementComponent>(
            ball,
            Vector2Normalize(get_rand_vec2(room_size)),
            // Speed should be negative because of vertical direction
            // This may cause issues later, but will do for now
            -(static_cast<float>(std::rand() % 200) + 10.0f));

        // Adding rectangle that fits our circle, for fast potential collision
        // detection via quadtree.
        registry.emplace<RectComp>(ball, RectComp{x, y, size * 2.0f, size * 2.0f});
    }
}

void Level::damage_player() {
    lifes--;
    life_counter.set_text(fmt::format("Lifes: {}", lifes));
    spdlog::info("Player HP has been decreased to {}", lifes);
    if (lifes <= 0) {
        gameover_screen.set_body_text(
            fmt::format("Final Score: {}\nBalloons Popped: {}", score, enemies_killed));
        is_gameover = true;
    }
}

void Level::kill_enemy(entt::entity entity) {
    spdlog::debug(
        "Destroying entity with id {}",
        // Converting to uint32_t coz it seems to be default type of entity id
        static_cast<std::uint32_t>(entity));
    registry.destroy(entity);
    enemies_left--;
}

void Level::resume() {
    is_paused = false;
}

void Level::exit_to_menu() {
    // parent->set_current_scene(new MainMenu(parent));
    must_close = true;
}

// Level stuff
Level::Level(SceneManager* p, Vector2 _room_size)
    : parent(p)
    , room_size(_room_size)
    , tree({0.0f, 0.0f, room_size.x, room_size.y}, check_collision)
    , pointer_size({10.0f, 10.0f})
    , pointer_rect({0.0f, 0.0f, pointer_size.x, pointer_size.y})
    , max_enemies(30) // TODO: rework this value to be based on Level's level.
    , enemies_left((std::rand() % (max_enemies - 10)) + 10)
    , enemies_killed(0)
    , score(0)
    , lifes(5)
    , score_counter(fmt::format("Score: {}", score), {10.0f, 10.0f})
    , life_counter(fmt::format("Lifes: {}", lifes), {10.0f, 40.0f})
    , kill_counter(fmt::format("Balloons Popped: {}", enemies_killed), {10.0f, 70.0f})
    , spawn_timer(3.5f)
    , gameover_screen("Game Over", "", std::bind(&Level::exit_to_menu, this))
    , pause_screen(
        "Paused",
        std::bind(&Level::resume, this),
        std::bind(&Level::exit_to_menu, this))
    , pause_button(
        shared::assets.sprites["cross_default"],
        shared::assets.sprites["cross_hover"],
        shared::assets.sprites["cross_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64}) {

    pause_button.set_pos({GetScreenWidth() - 64.0f, 0.0f});

    spawn_balls(enemies_left);
    spawn_timer.start();
}

Level::Level(SceneManager* p)
    : Level(
          p,
          {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}) {
}

void Level::update(float dt) {
    if (must_close) {
        parent->set_current_scene(new MainMenu(parent));
        return;
    }

    if (is_gameover) {
        gameover_screen.update();
    }
    else if (is_paused) {
        pause_screen.update();
    }
    else {
        pause_button.update();
        if (pause_button.is_clicked()) {
            is_paused = true;
        }

        update_collisions_tree();
        process_ball_collisions();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            process_mouse_collisions(GetMousePosition());
        };

        move_balls(dt);

        if (enemies_left < max_enemies) {
            if (spawn_timer.tick(dt)) {
                spawn_timer.start();
                int spawn_amount = (std::rand() % (max_enemies - enemies_left - 1)) + 1;
                enemies_left += spawn_amount;
                spawn_balls(spawn_amount);
            }
        }
    }
}

void Level::draw() {
    draw_balls();
    score_counter.draw();
    life_counter.draw();
    kill_counter.draw();
    pause_button.draw();

    if (is_gameover) {
        gameover_screen.draw();
    }
    else if (is_paused) {
        pause_screen.draw();
    }
}
