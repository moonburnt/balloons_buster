#include "level.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "common.hpp"
#include "event_screens.hpp"
#include "menus.hpp"
// For vector normalizaion
#include "entt/entity/fwd.hpp"
#include "raymath.h"
// For basic formatting
#include <cstddef>
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

const float ADDITIONAL_ROOM_HEIGHT = 100.0f;
const float CAMERA_MOVE_STEP = 30.0f;

// Our components.
class RectangleComponent {
protected:
    Vector2 size;
    Vector2 half_size;
    b2PolygonShape box;
    b2Body* rigid_body;
    Color color;

public:
    RectangleComponent(
        b2World* world,
        Vector2 pos,
        Vector2 _size,
        Color _color,
        float angle = 0,
        bool is_dynamic = true)
        : size(_size)
        , half_size({size.x/2, size.y/2})
        , color(_color) {

        b2BodyDef body_def;
        if (is_dynamic) {
            body_def.type = b2_dynamicBody;
        }
        else {
            body_def.type = b2_staticBody;
        }
        body_def.position.Set(pos.x, pos.y);
        body_def.angle = angle * DEG2RAD;
        body_def.userData.pointer = uintptr_t(this);
        rigid_body = world->CreateBody(&body_def);

        box.SetAsBox(half_size.x, half_size.y);

        b2FixtureDef fixture_def;
        fixture_def.shape = &box;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;

        rigid_body->CreateFixture(&fixture_def);
    }

    void draw() {
        DrawRectanglePro(
            {rigid_body->GetPosition().x, rigid_body->GetPosition().y, size.x, size.y},
            half_size,
            rigid_body->GetAngle() * RAD2DEG,
            color);
    }
};

class BallComponent {
protected:
    b2CircleShape circle_shape;
    b2Body* rigid_body;
    Color color;

public:
    float radius;

    BallComponent(
        b2World* world, Vector2 pos, float _radius, Color _color, b2Vec2 velocity)
        : color(_color)
        , radius(_radius) {
        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        body_def.position.Set(pos.x, pos.y);
        body_def.userData.pointer = uintptr_t(this);
        rigid_body = world->CreateBody(&body_def);

        circle_shape.m_radius = radius;
        b2FixtureDef fixture_def;
        fixture_def.shape = &circle_shape;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;

        rigid_body->CreateFixture(&fixture_def);

        rigid_body->SetAwake(true);

        // rigid_body->ApplyForceToCenter(velocity, true);
        rigid_body->SetLinearVelocity(velocity);

        // rigid_body->ApplyLinearImpulse(
        //     velocity, rigid_body->GetWorldCenter(), true);
    }

    // ~BallComponent() {
    //     b2World* world = rigid_body->GetWorld();
    //     world->DestroyBody(rigid_body);
    //     // rigid_body->SetUserData(NULL);
    // }

    void draw() {
        DrawCircleV(
            {rigid_body->GetPosition().x, rigid_body->GetPosition().y},
            radius,
            color);
    }
};

void Level::update_collisions_tree(float dt) {
    // Number are velocity iterations and position iterations.
    // TODO: figure out how these works
    world.Step(dt, 6, 2);
    // world.ClearForces();
}

void Level::spawn_walls() {
    const float thickness = 10.0f;
    const Color color = RED;

    entt::entity wall_bottom = registry.create();

    registry.emplace<RectangleComponent>(
        wall_bottom, &world,
        Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT},
        Vector2{static_cast<float>(GetScreenWidth()), thickness}, color,
        0, false);

    entt::entity wall_top = registry.create();
    registry.emplace<RectangleComponent>(
        wall_top, &world,
        Vector2{GetScreenWidth()/2.0f, -ADDITIONAL_ROOM_HEIGHT},
        Vector2{static_cast<float>(GetScreenWidth()), thickness}, color,
        0, false);

    entt::entity wall_left = registry.create();
    registry.emplace<RectangleComponent>(
        wall_left, &world,
        Vector2{0.0f, GetScreenHeight()/2.0f},
        Vector2{thickness, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT*2}, color,
        0, false);

    entt::entity wall_right = registry.create();
    registry.emplace<RectangleComponent>(
        wall_right, &world,
        Vector2{static_cast<float>(GetScreenWidth()), GetScreenHeight()/2.0f},
        Vector2{thickness, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT*2}, color,
        0, false);
}

void Level::draw_walls() {
    auto this_view = registry.view<RectangleComponent>();

    this_view.each([](auto& wall) {
        wall.draw();
    });
}

void Level::spawn_balls(int amount) {
    spdlog::debug("Attempting to spawn {} enemies", amount);
    for (int i = 0; i < amount; i++) {
        // First we need to initialize an empty entity with no components.
        // This will make registry assign an unique entity id to it and return it.
        entt::entity ball = registry.create();

        // Now lets initialize and attach all required components to our entity id.
        float x = static_cast<float>(std::rand() % static_cast<int>(room_size.x));
        float y = room_size.y + ADDITIONAL_ROOM_HEIGHT / 2;
        float size = static_cast<float>(std::rand() % 50) + 10.0f;

        float speed = static_cast<float>(std::rand() % 200) + 10.0f;
        Vector2 direction = Vector2Normalize(get_rand_vec2(room_size));

        registry.emplace<BallComponent>(
            ball, &world, Vector2{x, y}, size, BLUE,
            b2Vec2{direction.x*speed, -direction.y*speed});
    }
}

void Level::draw_balls() {
    auto this_view = registry.view<BallComponent>();

    this_view.each([](auto& ball) {
        ball.draw();
    });
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
    , world({0.0f, 0.0f}) // Values are gravity
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

    spawn_walls();

    spawn_balls(enemies_left);
    spawn_timer.start();

    camera.target = {0.0f, 0.0f};
    camera.zoom = 1.0f;
    camera.offset = {0.0f, 0.0f};
    camera.rotation = 0.0f;
}

Level::Level(SceneManager* p)
    : Level(
          p,
          {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}) {
}

Level::~Level() {
    // auto this_view = registry.view<BallComponent>();

    // this_view.each([](auto& ball) {
    //     delete ball;
    // });
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

        // Temporary stuff for debug purposes.
        // Not sure if camera will be movable at all in final game.
        if (IsKeyPressed(KEY_D)) {
            camera.target.x += CAMERA_MOVE_STEP;
        }
        else if (IsKeyPressed(KEY_A)) {
            camera.target.x -= CAMERA_MOVE_STEP;
        }
        else if (IsKeyPressed(KEY_W)) {
            camera.target.y -= CAMERA_MOVE_STEP;
        }
        else if (IsKeyPressed(KEY_S)) {
            camera.target.y += CAMERA_MOVE_STEP;
        }
        else if (IsKeyPressed(KEY_R)) {
            camera.target = {0.0f, 0.0f};
        }

        // update_collisions_tree(dt);

        // I'm not 100% sure what this does. But its been done like that in
        // raylib's examples, so I guess its a correct approach?
        accumulator += dt;
        while (accumulator >= phys_time) {
            accumulator -= phys_time;
            update_collisions_tree(phys_time);
        }
        // process_ball_collisions();

        // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        //     process_mouse_collisions(GetMousePosition());
        // };

        // move_balls(dt);

        if (enemies_left < max_enemies) {
            if (spawn_timer.tick(dt)) {
                spawn_timer.start();

                // This has a chance to cause division by zero.
                // Thus I've replaced it with garbage below, for now
                // int spawn_amount = (std::rand() % (max_enemies - enemies_left - 1)) + 1;
                int spawn_amount;
                int spawn_diff = max_enemies - enemies_left;
                if (spawn_diff > 1) {
                    spawn_amount = (std::rand() % spawn_diff - 1) + 1;
                }
                else {
                    spawn_amount = 1;
                }

                enemies_left += spawn_amount;
                spawn_balls(spawn_amount);
            }
        }
    }
}

void Level::draw() {
    BeginMode2D(camera);
    draw_walls();
    draw_balls();
    EndMode2D();

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
