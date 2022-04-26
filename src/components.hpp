#pragma once

#include <box2d/b2_settings.h>
#include <box2d/box2d.h>
#include <entt/entity/fwd.hpp>
#include <raylib.h>

// Our components.

struct FixtureUserData {
    entt::entity entity;
};

struct RectangleComponent {
    b2PolygonShape box;
    Vector2 half_size;
    Vector2 size;
};

struct BallComponent {
    b2CircleShape circle_shape;
};

struct PhysicsBodyComponent
{
    FixtureUserData user_data;
    b2Body* body;
};

struct ColorComponent
{
    Color color;
    ColorComponent(const Color& color);
};
