#include "components.hpp"

ColorComponent::ColorComponent(const Color& color)
    : color(color) {}

PhysicsBodyComponent::PhysicsBodyComponent()
    : user_data(std::make_unique<FixtureUserData>()) {}
