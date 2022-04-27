#include "level.hpp"

#include "app.hpp"
#include "event_screens.hpp"
#include "common.hpp"
#include "components.hpp"
#include "menus.hpp"

#include <box2d/b2_world_callbacks.h>

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>

#include <fmt/core.h>

#include <raylib.h>
#include <raymath.h>

#include <spdlog/spdlog.h>

const float ADDITIONAL_ROOM_HEIGHT = 100.0f;
const float CAMERA_MOVE_STEP = 30.0f;

class CollisionQueryCallback : public b2QueryCallback {
public:
    std::vector<entt::entity> collisions;

    bool ReportFixture(b2Fixture* fixture_def) override {
        auto user_data = reinterpret_cast<FixtureUserData*>(
            fixture_def->GetUserData().pointer);
        collisions.push_back(user_data->entity);

        return true;
    }
};

void Level::update_collisions_tree(float dt) {
    // Numbers are velocity iterations and position iterations.
    // TODO: figure out how these works
    world.Step(dt, 6, 2);
    // world.ClearForces();
}

void Level::process_mouse_collisions(Vector2 mouse_pos) {
    b2AABB mouse_rect = {{mouse_pos.x, mouse_pos.y}, {mouse_pos.x, mouse_pos.y}};

    CollisionQueryCallback query;
    world.QueryAABB(&query, mouse_rect);

    if (query.collisions.size() == 0) {
        return;
    }

    std::vector<entt::entity> to_remove;
    for (auto entity : query.collisions) {
        if (entity == entt::null) {
            spdlog::error(
                "Collision returned null entity {}",
                static_cast<uint32_t>(entity));
            ASSERT(false);
        }

        spdlog::info(
            "Mouse Pointer collides with {}",
            static_cast<uint32_t>(entity));

        const auto ball = registry.try_get<BallComponent>(entity);
        if (ball != nullptr) {
            to_remove.push_back(entity);
        }
    }

    for (auto e : to_remove) {
        spdlog::info("destroying entity {}", static_cast<uint32_t>(e));
        registry.destroy(e);
    }
}

void Level::spawn_walls() {
    const float thickness = 10.0f;

    const Vector2 positions[4] = {
        {GetScreenWidth()/2.0f, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT},
        {GetScreenWidth()/2.0f, -ADDITIONAL_ROOM_HEIGHT},
        {0.0f, GetScreenHeight()/2.0f},
        {static_cast<float>(GetScreenWidth()), GetScreenHeight()/2.0f},
    };

    const Vector2 sizes[4] = {
        {static_cast<float>(GetScreenWidth()), thickness},
        {static_cast<float>(GetScreenWidth()), thickness},
        {thickness, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT*2},
        {thickness, GetScreenHeight()+ADDITIONAL_ROOM_HEIGHT*2},
    };

    for (auto i = 0u; i < 4; ++i) {
        entt::entity wall = registry.create();
        auto& rect_comp = registry.emplace<RectangleComponent>(wall);
        auto& phys_comp = registry.emplace<PhysicsBodyComponent>(wall);
        registry.emplace<ColorComponent>(wall, RED);
        phys_comp.user_data.entity = wall;

        b2BodyDef body_def;
        body_def.type = b2_staticBody;

        const auto& pos = positions[i];
        body_def.position.Set(pos.x, pos.y);
        body_def.angle = 0.0f;
        phys_comp.body = world.CreateBody(&body_def);

        auto half_size = sizes[i];
        half_size.x *= 0.5f;
        half_size.y *= 0.5f;

        rect_comp.box.SetAsBox(half_size.x, half_size.y);
        rect_comp.size = sizes[i];
        rect_comp.half_size = half_size;

        b2FixtureDef fixture_def;
        fixture_def.shape = &rect_comp.box;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(&phys_comp.user_data);

        phys_comp.body->CreateFixture(&fixture_def);
    }
}

void Level::draw_walls() {
    auto view = registry.view<RectangleComponent, ColorComponent, PhysicsBodyComponent>();

    view.each([](auto, auto& rect, auto& color, auto& phys) {
        DrawRectanglePro(
            {phys.body->GetPosition().x, phys.body->GetPosition().y, rect.size.x, rect.size.y},
            rect.half_size,
            phys.body->GetAngle() * RAD2DEG,
            color.color);
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

        auto& ball_comp = registry.emplace<BallComponent>(ball);
        auto& phys_body = registry.emplace<PhysicsBodyComponent>(ball);
        registry.emplace<ColorComponent>(ball, BLUE);
        phys_body.user_data.entity = ball;

        ball_comp.circle_shape.m_radius = size;

        b2FixtureDef fixture_def;
        fixture_def.shape = &ball_comp.circle_shape;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(&phys_body.user_data);

        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        const auto pos = Vector2{x, y};
        body_def.position.Set(pos.x, pos.y);

        phys_body.body = world.CreateBody(&body_def);
        phys_body.body->CreateFixture(&fixture_def);
        phys_body.body->SetAwake(true);
        const auto velocity = b2Vec2{direction.x*speed, -direction.y*speed};
        phys_body.body->SetLinearVelocity(velocity);
    }
}

void Level::draw_balls() {
    auto view = registry.view<BallComponent, ColorComponent, PhysicsBodyComponent>();

    view.each([](auto, auto& ball, auto& color, auto& phys) {
        DrawCircleV(
            {phys.body->GetPosition().x, phys.body->GetPosition().y},
            ball.circle_shape.m_radius,
            color.color);
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

void Level::cleanup_physics(entt::registry& reg, entt::entity e) {
    spdlog::info("Deleting body component of entity {}", static_cast<uint32_t>(e));
    auto comp = reg.get<PhysicsBodyComponent>(e);
    world.DestroyBody(comp.body);
}

// Level stuff
Level::Level(App* app, SceneManager* p, Vector2 _room_size)
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
    , gameover_screen(app, "Game Over", "", std::bind(&Level::exit_to_menu, this))
    , pause_screen(
        app,
        "Paused",
        std::bind(&Level::resume, this),
        std::bind(&Level::exit_to_menu, this))
    , pause_button(
        app->assets.sprites["cross_default"],
        app->assets.sprites["cross_hover"],
        app->assets.sprites["cross_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64})
    , app(app) {

    pause_button.set_pos({GetScreenWidth() - 64.0f, 0.0f});

    spawn_walls();

    spawn_balls(enemies_left);
    spawn_timer.start();

    camera.target = {0.0f, 0.0f};
    camera.zoom = 1.0f;
    camera.offset = {0.0f, 0.0f};
    camera.rotation = 0.0f;

    registry.on_destroy<PhysicsBodyComponent>().connect<&Level::cleanup_physics>(this);
}

Level::Level(App* app, SceneManager* p)
    : Level(app,
          p,
          {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}) {
}

void Level::update(float dt) {
    if (must_close) {
        parent->set_current_scene(new MainMenu(app, parent));
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            process_mouse_collisions(GetMousePosition());
        };

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
