#pragma once

#include <box2d/b2_settings.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <raylib.h>

// Our components.

struct FixtureUserData {
    entt::entity entity;
    const entt::registry* registry;
};

struct RectangleComponent {
    Vector2 half_size;
    Vector2 size;
};

struct BallComponent {
    float radius;
};

struct PhysicsBodyComponent {
    std::unique_ptr<FixtureUserData> user_data;
    b2Body* body;
    PhysicsBodyComponent();
};

struct ColorComponent {
    Color color;
    ColorComponent(const Color& color);
};

struct HealthComponent {
    int health;
};
