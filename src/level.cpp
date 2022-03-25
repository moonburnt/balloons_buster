#include "level.hpp"
// For vector normalizaion
#include "raymath.h"
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

void process_collisions(entt::registry& registry, Vector2 mouse_pos) {
    // This may go wrong if PosComp will ever be attached to things that should
    // not collide with player's pointer
    auto this_view = registry.view<PosComp, BallComponent>();

    for (auto entity : this_view) {
        PosComp& pos = this_view.get<PosComp>(entity);
        BallComponent& ball = this_view.get<BallComponent>(entity);
        if (CheckCollisionPointCircle(mouse_pos, pos, ball.radius)) {
            // TODO: add death animation, schedule destroying
            registry.destroy(entity);
        };
    };
}

void move_balls(entt::registry& registry, int room_x, int room_y, float dt) {
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
        if (pos.x < 0 || pos.x > room_x) {
            movement.direction.x = -movement.direction.x;
            direction_changed = true;
        }
        if (pos.y < 0 || pos.y > room_y) {
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

// Returns random Vector2 with values between 0 and provided
Vector2 get_rand_vec2(int x, int y) {
    return Vector2{
        static_cast<float>(std::rand() % x),
        static_cast<float>(std::rand() % y)};
}

// Returns random non-white color
Color get_rand_color() {
    int rgb[3];

    for (int i = 0; i < 3; i++) {
        rgb[i] = std::rand() % 255;
    }

    if (rgb[0] == 0 && rgb[1] == 0 && rgb[2] == 0) {
        rgb[std::rand() % 3] = 10;
    }

    return Color{
        static_cast<unsigned char>(rgb[0]),
        static_cast<unsigned char>(rgb[1]),
        static_cast<unsigned char>(rgb[2]),
        255};
}

// Level stuff
Level::Level(int x, int y)
    : level_x(x)
    , level_y(y) {
    // Now, lets create our entities. Say, about 10 ~ 30 would be ok?
    const int balls_amount = (std::rand() % 20) + 10;
    for (int i = 0; i < balls_amount; i++) {
        // First we need to initialize an empty entity with no components.
        // This will make registry assign an unique entity id to it and return it.
        entt::entity ball = registry.create();

        // Now lets initialize and attach all required components to our entity id.
        registry.emplace<PosComp>(ball, get_rand_vec2(level_x, level_y));
        registry.emplace<BallComponent>(
            ball,
            get_rand_color(),
            static_cast<float>(std::rand() % 50) + 10.0f);

        float spd = static_cast<float>(std::rand() % 200);
        // Because speed is constant in our example, we could simply use it to
        // determine if we need movement component at all or not
        if (spd) {
            registry.emplace<MovementComponent>(
                ball,
                Vector2Normalize(get_rand_vec2(level_x, level_y)),
                spd);
        };
    }
}

Level::Level()
    : Level(GetScreenWidth(), GetScreenHeight()) {
}

void Level::update(float dt) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        process_collisions(registry, GetMousePosition());
    };

    move_balls(registry, level_x, level_y, dt);
}

void Level::draw() {
    draw_balls(registry);
}
