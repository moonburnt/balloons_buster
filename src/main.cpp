#include "raylib.h"
// For vector normalizaion
#include "raymath.h"
#include <entt/entity/registry.hpp>
// For rand()
#include <cstdlib>
#include "engine.hpp"
#include <string>

// Our components.
// BallComponent is a basic component to draw a circle
struct BallComponent {
    Color color;
    float radius;
    Vector2 position;
};

// MovementCompoment. Since BallComponent already contains position, doesn't
// feature it.
struct MovementComponent {
    Vector2 direction;
    float speed;
};

void move_balls(entt::registry &registry, int room_x, int room_y, float dt) {
    auto this_view = registry.view<BallComponent, MovementComponent>();

    // There are many ways to iterate through components in EnTT.
    // For the sake of simplicity, we will use a simple for loop there.
    for (auto entity : this_view) {
        MovementComponent &movement = this_view.get<MovementComponent>(entity);
        BallComponent &ball = this_view.get<BallComponent>(entity);

        ball.position.x += movement.direction.x * (movement.speed * dt);
        ball.position.y += movement.direction.y * (movement.speed * dt);

        // Ensuring balls will never leave screen's area
        bool direction_changed = false;
        if (ball.position.x < 0 || ball.position.x > room_x) {
            movement.direction.x = -movement.direction.x;
            direction_changed = true;
        }
        if (ball.position.y < 0 || ball.position.y > room_y) {
            movement.direction.y = -movement.direction.y;
            direction_changed = true;
        }

        // In case some changes has been made to an entity - we would need to
        // replace the stored component.
        if (direction_changed) {
            registry.replace<MovementComponent>(entity, movement);
        }

        registry.replace<BallComponent>(entity, ball);
    }
}

void draw_balls(entt::registry &registry) {
    // In order to iterate through components, we must make a view - an iterator
    // that will grab specified elements from storage.
    auto this_view = registry.view<BallComponent>();

    // Lets iterate with callback there.
    this_view.each([](const auto &ball) {
        DrawCircleV(ball.position, ball.radius, ball.color);
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

class Level : public Scene {
    private:
        // Level's registry that will hold our entities.
        entt::registry registry;

        int level_x;
        int level_y;

    public:
        Level(int x, int y) : level_x(x), level_y(y) {
            // Now, lets create our entities. Say, about 10 ~ 30 would be ok?
            const int balls_amount = (std::rand() % 20) + 10;
            for (int i = 0; i < balls_amount; i++) {
                // First we need to initialize an empty entity with no components.
                // This will make registry assign an unique entity id to it and return it.
                auto ball = registry.create();

                // Now lets initialize and attach all required components to our entity id.
                Vector2 pos = get_rand_vec2(level_x, level_y);
                Vector2 direction = Vector2Normalize(get_rand_vec2(level_x, level_y));

                registry.emplace<BallComponent>(
                    ball,
                    get_rand_color(),
                    static_cast<float>(std::rand() % 50) + 10.0f,
                    pos);

                float spd = static_cast<float>(std::rand() % 200);
                // Because speed is constant in our example, we could simply use it to
                // determine if we need movement component at all or not
                if (spd) {
                    registry.emplace<MovementComponent>(ball, direction, spd);
                };
            }
        }

        Level() : Level(GetScreenWidth(), GetScreenHeight()) {}

        void update(float dt) override {
            move_balls(registry, level_x, level_y, dt);
        }

        void draw() override {
            draw_balls(registry);
        }
};

int main() {
    GameWindow window;
    window.init(1280, 720, "Balloon Buster");
    window.sc_mgr.set_current_scene(new Level());
    window.run();

    return 0;
}
