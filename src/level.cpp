#include "level.hpp"
#include "common.hpp"
// For vector normalizaion
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

// BallComponent. Require Vector2 to work
struct BallComponent {
    Color color;
    float radius;
};

// MovementCompoment. Require Vector2 to work
struct MovementComponent {
    Vector2 direction;
    float speed;
};

void process_collisions(
    entt::registry& registry, Vector2 mouse_pos, int& enemies_left, int& score) {
    // This may go wrong if PosComp will ever be attached to things that should
    // not collide with player's pointer
    auto this_view = registry.view<PosComp, BallComponent>();

    for (auto entity : this_view) {
        PosComp& pos = this_view.get<PosComp>(entity);
        BallComponent& ball = this_view.get<BallComponent>(entity);
        if (CheckCollisionPointCircle(mouse_pos, pos, ball.radius)) {
            spdlog::debug(
                "Destroying entity with id {}",
                // Converting to uint32_t coz it seems to be default type of entity id
                static_cast<std::uint32_t>(entity));
            // TODO: add death animation, schedule destroying
            registry.destroy(entity);
            enemies_left--;
            // TODO: "RewardComp" with different score values depending on type
            score += 10;
            // For now, its only possible to pop one balloon at once
            return;
        };
    };
}

void move_balls(entt::registry& registry, Vector2 room_size, float dt) {
    auto this_view = registry.view<PosComp, MovementComponent>();

    // There are many ways to iterate through components in EnTT.
    // For the sake of simplicity, we will use a simple for loop there.
    for (auto entity : this_view) {
        MovementComponent& movement = this_view.get<MovementComponent>(entity);
        PosComp& pos = this_view.get<PosComp>(entity);

        pos.x += movement.direction.x * (movement.speed * dt);
        pos.y += movement.direction.y * (movement.speed * dt);

        // Ensuring balls will never leave screen's area
        bool direction_changed = false;
        if (pos.x < 0 || pos.x > room_size.x) {
            movement.direction.x = -movement.direction.x;
            direction_changed = true;
        }
        if (pos.y < 0 || pos.y > room_size.y) {
            movement.direction.y = -movement.direction.y;
            direction_changed = true;
        }

        // In case some changes has been made to an entity - we would need to
        // replace the stored component.
        if (direction_changed) {
            registry.replace<MovementComponent>(entity, movement);
        }

        registry.replace<PosComp>(entity, pos);
    }
}

void draw_balls(entt::registry& registry) {
    // In order to iterate through components, we must make a view - an iterator
    // that will grab specified elements from storage.
    auto this_view = registry.view<PosComp, BallComponent>();

    // Lets iterate with callback there.
    this_view.each([](const auto& pos, const auto& ball) {
        DrawCircleV(pos, ball.radius, ball.color);
    });
}

void spawn_balls(entt::registry& registry, Vector2 room_size, int amount) {
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
                room_size.y});
        registry.emplace<BallComponent>(
            ball,
            get_rand_color(),
            static_cast<float>(std::rand() % 50) + 10.0f);
        registry.emplace<MovementComponent>(
            ball,
            Vector2Normalize(get_rand_vec2(room_size)),
            static_cast<float>(std::rand() % 200) + 10.0f);
    }
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
    spawn_balls(registry, room_size, enemies_left);
    spawn_timer.start();
}

Level::Level()
    : Level(
          {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}) {
}

void Level::update(float dt) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        process_collisions(registry, GetMousePosition(), enemies_left, score);
        score_counter.set_text(fmt::format("Score: {}", score));
        life_counter.set_text(fmt::format("Lifes: {}", lifes));
    };

    move_balls(registry, room_size, dt);

    if (enemies_left < max_enemies) {
        if (spawn_timer.tick(dt)) {
            spawn_timer.start();
            int spawn_amount = (std::rand() % (max_enemies - enemies_left - 1)) + 1;
            enemies_left += spawn_amount;
            spawn_balls(registry, room_size, spawn_amount);
        }
    }
}

void Level::draw() {
    draw_balls(registry);
    score_counter.draw();
    life_counter.draw();
}
