#include "components.hpp"
#include "entt/entity/fwd.hpp"

// Base
CollisionComponentBase::CollisionComponentBase(Color c, entt::entity eid)
    : color(c)
    , entity_id(eid) {}

// Rect
RectangleComponent::RectangleComponent(
        b2World* world,
        entt::entity eid,
        Vector2 pos,
        Vector2 _size,
        Color _color,
        float angle,
        bool is_dynamic)
        : CollisionComponentBase(_color, eid)
        , size(_size)
        , half_size({size.x/2, size.y/2}) {

        b2BodyDef body_def;
        if (is_dynamic) {
            body_def.type = b2_dynamicBody;
        }
        else {
            body_def.type = b2_staticBody;
        }
        body_def.position.Set(pos.x, pos.y);
        body_def.angle = angle * DEG2RAD;
        // body_def.userData.pointer = uintptr_t(this);
        body = world->CreateBody(&body_def);

        box.SetAsBox(half_size.x, half_size.y);

        b2FixtureDef fixture_def;
        fixture_def.shape = &box;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        fixture_def.userData.pointer = uintptr_t(this);

        body->CreateFixture(&fixture_def);
    }

void RectangleComponent::draw() {
    DrawRectanglePro(
        {body->GetPosition().x, body->GetPosition().y, size.x, size.y},
        half_size,
        body->GetAngle() * RAD2DEG,
        color);
}

// Ball
BallComponent::BallComponent(
    b2World* world,
    entt::entity eid,
    Vector2 pos,
    float _radius,
    Color _color,
    b2Vec2 velocity)
    : CollisionComponentBase(_color, eid)
    , radius(_radius) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(pos.x, pos.y);
    // body_def.userData.pointer = uintptr_t(this);
    body = world->CreateBody(&body_def);

    circle_shape.m_radius = radius;
    b2FixtureDef fixture_def;
    fixture_def.shape = &circle_shape;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.3f;
    fixture_def.userData.pointer = uintptr_t(this);

    body->CreateFixture(&fixture_def);

    body->SetAwake(true);

    // body->ApplyForceToCenter(velocity, true);
    body->SetLinearVelocity(velocity);

    // body->ApplyLinearImpulse(
    //     velocity, body->GetWorldCenter(), true);
}

void BallComponent::draw() {
    DrawCircleV(
        {body->GetPosition().x, body->GetPosition().y},
        radius,
        color);
}
