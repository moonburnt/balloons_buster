#pragma once

#include "box2d/box2d.h"
#include "entt/entity/fwd.hpp"
#include "raylib.h"

// Our components.

class CollisionComponentBase {
// protected:
    // b2Body* body;
    // Color color;

public:
    b2Body* body;
    Color color;
    entt::entity entity_id;

    CollisionComponentBase(Color color, entt::entity eid);
};

class RectangleComponent : public CollisionComponentBase {
protected:
    Vector2 size;
    Vector2 half_size;
    b2PolygonShape box;

public:
    RectangleComponent(
        b2World* world,
        entt::entity eid,
        Vector2 pos,
        Vector2 size,
        Color color,
        float angle = 0,
        bool is_dynamic = true);

    void draw();
};

class BallComponent : public CollisionComponentBase {
protected:
    b2CircleShape circle_shape;

public:
    float radius;

    BallComponent(
        b2World* world,
        entt::entity eid,
        Vector2 pos,
        float _radius,
        Color _color,
        b2Vec2 velocity);

    // ~BallComponent() {
    //     b2World* world = rigid_body->GetWorld();
    //     world->DestroyBody(rigid_body);
    //     // rigid_body->SetUserData(NULL);
    // }

    void draw();
};
