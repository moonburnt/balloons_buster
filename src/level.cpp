#include "level.hpp"
#include "common.hpp"
// For vector normalizaion
#include "entt/entity/fwd.hpp"
#include "raymath.h"
// For basic formatting
#include <fmt/core.h>
// For logging
#include "spdlog/spdlog.h"
// To log entity
#include <cinttypes>
// For rand()
#include <cstdlib>

// Our components.
// Position component will be just Vector2 - no need to specify it, but we can
// typedef an alias, to make it easier to understand.
// TODO: maybe revert it back to be part of BallComponent?
typedef Vector2 PosComp;

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

void Level::process_collisions(Vector2 mouse_pos) {
    // This may go wrong if PosComp will ever be attached to things that should
    // not collide with player's pointer
    auto this_view = registry.view<PosComp, BallComponent>();

    for (auto entity : this_view) {
        PosComp& pos = this_view.get<PosComp>(entity);
        BallComponent& ball = this_view.get<BallComponent>(entity);
        if (CheckCollisionPointCircle(mouse_pos, pos, ball.radius)) {
            // TODO: add death animation, schedule destroying
            kill_enemy(entity);
            // TODO: "RewardComp" with different score values depending on type
            score += 10;
            score_counter.set_text(fmt::format("Score: {}", score));
            // For now, its only possible to pop one balloon at once
            return;
        };
    };
}

void Level::move_balls(float dt) {
    auto this_view = registry.view<PosComp, MovementComponent>();

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

        registry.replace<PosComp>(entity, pos);
    }
}

void Level::draw_balls() {
    // In order to iterate through components, we must make a view - an iterator
    // that will grab specified elements from storage.
    auto this_view = registry.view<PosComp, BallComponent>();

    // Lets iterate with callback there.
    this_view.each([](const auto& pos, const auto& ball) {
        DrawCircleV(pos, ball.radius, ball.color);
    });
}

void Level::spawn_balls(int amount) {
    spdlog::debug("Attempting to spawn {} enemies", amount);
    for (int i = 0; i < amount; i++) {
        // First we need to initialize an empty entity with no components.
        // This will make registry assign an unique entity id to it and return it.
        entt::entity ball = registry.create();

        // Now lets initialize and attach all required components to our entity id.
        registry.emplace<PosComp>(
            ball,
            PosComp{
                static_cast<float>(std::rand() % static_cast<int>(room_size.x)),
                room_size.y + 50.0f});
        registry.emplace<BallComponent>(
            ball,
            get_rand_color(),
            static_cast<float>(std::rand() % 50) + 10.0f);
        registry.emplace<MovementComponent>(
            ball,
            Vector2Normalize(get_rand_vec2(room_size)),
            // Speed should be negative because of vertical direction
            // This may cause issues later, but will do for now
            -(static_cast<float>(std::rand() % 200) + 10.0f));
    }
}

void Level::damage_player() {
    lifes--;
    life_counter.set_text(fmt::format("Lifes: {}", lifes));
    spdlog::info("Player HP has been decreased to {}", lifes);
    // TODO: show gameover if hp falls below 0
}

void Level::kill_enemy(entt::entity entity) {
    spdlog::debug(
        "Destroying entity with id {}",
        // Converting to uint32_t coz it seems to be default type of entity id
        static_cast<std::uint32_t>(entity));
    registry.destroy(entity);
    enemies_left--;
}

// Level stuff
Level::Level(Vector2 _room_size)
    : room_size(_room_size)
    , max_enemies(30) // TODO: rework this value to be based on Level's level.
    , enemies_left((std::rand() % (max_enemies - 10)) + 10)
    , score(0)
    , lifes(5)
    , score_counter(fmt::format("Score: {}", score), {10.0f, 10.0f})
    , life_counter(fmt::format("Lifes: {}", lifes), {10.0f, 40.0f})
    , spawn_timer(3.5f) {
    spawn_balls(enemies_left);
    spawn_timer.start();
}

Level::Level()
    : Level(
          {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}) {
}

void Level::update(float dt) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        process_collisions(GetMousePosition());
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

void Level::draw() {
    draw_balls();
    score_counter.draw();
    life_counter.draw();
}
