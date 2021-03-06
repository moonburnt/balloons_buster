#include "level.hpp"

#include "app.hpp"
#include "box2d/b2_math.h"
#include "engine/utility.hpp"
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
#include <random>

#include <spdlog/spdlog.h>

const float ADDITIONAL_ROOM_HEIGHT = 100.0f;
const float CAMERA_MOVE_STEP = 30.0f;

Wind::Wind(
    b2World* world,
    float min_timer_length,
    float max_timer_length,
    float min_power,
    float max_power)
    : world(world)
    , min_timer_length(min_timer_length)
    , max_timer_length(max_timer_length)
    , min_power(min_power)
    , max_power(max_power)
    , timer(min_timer_length) {
    timer.start();
}

void Wind::blow(b2Vec2 wind) {
    spdlog::info("Blowing wind with {}, {} power", wind.x, wind.y);
    b2Body* last_body = world->GetBodyList();

    int i = 0;
    while(last_body != nullptr) {
        i++;
        // This should be the right way but it did not work, for some reason
        // last_body->ApplyForceToCenter(wind, true);

        // Thus temporary using this thing. Also keep in mind that we have no
        // "weight" now, so things may move weirdly (e.g do so in perfect sync).
        last_body->SetLinearVelocity(wind);
        last_body = last_body->GetNext();
    }
    spdlog::info("Wind affected {} targets", i);
}

void Wind::update(float dt) {
    if (timer.tick(dt)) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> timer_dis(
            min_timer_length, max_timer_length);

        std::uniform_real_distribution<float> power_dis(min_power, max_power);
        float h_power = power_dis(gen);
        float v_power = power_dis(gen);

        if (randbool()) {
            h_power = -h_power;
        }
        if (randbool()) {
            v_power = -v_power;
        }

        blow({h_power, v_power});
        timer = Timer(timer_dis(gen));
        timer.start();
    }
}

class CollisionQueryCallback : public b2QueryCallback {
public:
    std::vector<entt::entity> collisions;

    bool ReportFixture(b2Fixture* fixture_def) override {
        const auto body = fixture_def->GetBody();
        if (body->GetType() == b2_staticBody)
        {
            return true;
        }

        auto user_data = reinterpret_cast<FixtureUserData*>(
            fixture_def->GetUserData().pointer);

        spdlog::info("Collided with body {}, entity {}", reinterpret_cast<uint64_t>(body), static_cast<uint32_t>(user_data->entity));
        auto registry = user_data->registry;
        ASSERT(registry != nullptr);
        ASSERT(registry->valid(user_data->entity));

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

    // TODO: make damage points customizable
    int dmg = 1;
    for (auto entity : query.collisions) {
        if (entity == entt::null) {
            spdlog::error(
                "Collision returned null entity {}",
                static_cast<uint32_t>(entity));
            ASSERT(false);
        }

        spdlog::debug(
            "Mouse Pointer collides with {}",
            static_cast<uint32_t>(entity));

        const auto ball = registry.try_get<BallComponent>(entity);
        if (ball != nullptr) {
            auto& hp = registry.get<HealthComponent>(entity);
            if (hp.health - dmg <= 0) {
                spdlog::debug(
                    "Scheduling entity {} to be removed",
                    static_cast<uint32_t>(entity));
                to_remove.push_back(entity);
                enemies_left--;
                enemies_killed++;
                score += 15;
                kill_counter.set_text(
                    fmt::format("Balloons Popped: {}", enemies_killed));
            }
            else {
                spdlog::debug(
                    "Dealing {} damage to entity {}",
                    dmg,
                    static_cast<uint32_t>(entity));
                hp.health -= dmg;
                score += 5;
            }
            score_counter.set_text(fmt::format("Score: {}", score));
        }
    }

    for (auto e : to_remove) {
        spdlog::info("Destroying entity {}", static_cast<uint32_t>(e));
        registry.destroy(e);
    }

    validate_physics();
}

void Level::spawn_walls() {
    const float thickness = 10.0f;

    const Vector2 positions[4] = {
        {get_window_width()/2.0f, get_window_height()+ADDITIONAL_ROOM_HEIGHT},
        {get_window_width()/2.0f, -ADDITIONAL_ROOM_HEIGHT},
        {0.0f, get_window_height()/2.0f},
        {static_cast<float>(get_window_width()), get_window_height()/2.0f},
    };

    const Vector2 sizes[4] = {
        {static_cast<float>(get_window_width()), thickness},
        {static_cast<float>(get_window_width()), thickness},
        {thickness, get_window_height()+ADDITIONAL_ROOM_HEIGHT*2},
        {thickness, get_window_height()+ADDITIONAL_ROOM_HEIGHT*2},
    };

    for (auto i = 0u; i < 4; ++i) {
        entt::entity wall = registry.create();
        auto& rect_comp = registry.emplace<RectangleComponent>(wall);
        auto& phys_comp = registry.emplace<PhysicsBodyComponent>(wall);
        registry.emplace<ColorComponent>(wall, RED);
        phys_comp.user_data->entity = wall;
        phys_comp.user_data->registry = &registry;

        b2BodyDef body_def;
        body_def.type = b2_staticBody;

        const auto& pos = positions[i];
        body_def.position.Set(pos.x, pos.y);
        body_def.angle = 0.0f;
        phys_comp.body = world.CreateBody(&body_def);

        auto half_size = sizes[i];
        half_size.x *= 0.5f;
        half_size.y *= 0.5f;

        b2PolygonShape box;
        box.SetAsBox(half_size.x, half_size.y);
        rect_comp.size = sizes[i];
        rect_comp.half_size = half_size;

        b2FixtureDef fixture_def;
        fixture_def.shape = &box;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(phys_comp.user_data.get());

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

        auto& ball_comp = registry.emplace<BallComponent>(ball);

        // Set ball's HP to 1. TODO: add customization options.
        registry.emplace<HealthComponent>(ball, 1);

        auto& phys_body = registry.emplace<PhysicsBodyComponent>(ball);
        registry.emplace<ColorComponent>(ball, BLUE);
        phys_body.user_data->entity = ball;
        phys_body.user_data->registry = &registry;

        b2CircleShape circle_shape;
        circle_shape.m_radius = size;
        ball_comp.radius = size;

        b2FixtureDef fixture_def;
        fixture_def.shape = &circle_shape;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(phys_body.user_data.get());

        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        const auto pos = Vector2{x, y};
        body_def.position.Set(pos.x, pos.y);

        phys_body.body = world.CreateBody(&body_def);
        phys_body.body->CreateFixture(&fixture_def);
        // A lazy way to make balloon float upwards.
        // Does not have anything like weight, it probably affected by gravity
        // itself (e.g will move things upwards faster if gravity is higher.
        phys_body.body->SetGravityScale(-1.0f);
        // phys_body.body->SetAwake(true);
    }

    validate_physics();
}

void Level::draw_balls() {
    auto view = registry.view<BallComponent, ColorComponent, PhysicsBodyComponent>();

    view.each([](auto, auto& ball, auto& color, auto& phys) {
        DrawCircleV(
            {phys.body->GetPosition().x, phys.body->GetPosition().y},
            ball.radius,
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

void Level::resume() {
    is_paused = false;
}

void Level::exit_to_menu() {
    // parent->set_current_scene(new MainMenu(parent));
    must_close = true;
}

void Level::cleanup_physics(entt::registry& reg, entt::entity e) {
    spdlog::info("Deleting body component of entity {}", static_cast<uint32_t>(e));
    const auto& comp = reg.get<PhysicsBodyComponent>(e);
    world.DestroyBody(comp.body);
}

void Level::validate_physics() {
    auto body = world.GetBodyList();
    std::vector<b2Body*> physics_bodies;
    std::vector<b2Fixture*> fixtures;
    std::vector<entt::entity> physics_entities;
    std::vector<FixtureUserData*> physics_user_data;
    while (body != nullptr) {
        physics_bodies.push_back(body);
        auto fixture = body->GetFixtureList();
        fixtures.push_back(fixture);
        auto data = reinterpret_cast<FixtureUserData*>(fixture->GetUserData().pointer);
        physics_user_data.push_back(data);
        physics_entities.push_back(data->entity);
        body = body->GetNext();
    }

    std::vector<entt::entity> user_data_entities;
    std::vector<entt::entity> alive_entities;
    std::vector<b2Body*> physics_bodies_from_component;
    std::vector<FixtureUserData*> user_data;
    auto view = registry.view<PhysicsBodyComponent>();
    for (auto e : view) {
        alive_entities.push_back(e);
        auto [body] = view.get(e);
        user_data.push_back(body.user_data.get());
        physics_bodies_from_component.push_back(body.body);
        user_data_entities.push_back(body.user_data->entity);
    }

    for (auto e : alive_entities) {
        ASSERT(registry.valid(e));
        auto [body] = view.get(e);
        ASSERT(registry.valid(body.user_data->entity));
        ASSERT(std::find(physics_bodies.begin(), physics_bodies.end(), body.body) != physics_bodies.end());
        ASSERT(std::find(physics_entities.begin(), physics_entities.end(), e) != physics_entities.end());
    }
}

// Level stuff
Level::Level(App* app, SceneManager* p, Vector2 _room_size)
    : parent(p)
    , room_size(_room_size)
    , world({0.0f, 6.0f}) // Values are gravity, horizontal and vertical
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
    , pause_button()
    , app(app)
    // TODO: set min/max timer and power values depending on level's difficulty
    , wind(&world, 3.0f, 5.0f, 100.0f, 300.0f) {

    GuiBuilder gb = GuiBuilder(app);

    pause_button = gb.make_close_button();

    pause_button->set_pos({get_window_width() - 64.0f, 0.0f});

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
          {static_cast<float>(get_window_width()), static_cast<float>(get_window_height())}) {
}

Level::~Level() {
    delete pause_button;
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
        pause_button->update();
        if (pause_button->is_clicked()) {
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

        wind.update(dt);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            process_mouse_collisions(GetScreenToWorld2D(GetMousePosition(), camera));
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
    pause_button->draw();

    if (is_gameover) {
        gameover_screen.draw();
    }
    else if (is_paused) {
        pause_screen.draw();
    }
}
