#include "transform_component.hpp"

namespace engine::component {
TransformComponent::TransformComponent(sf::Vector2f position, sf::Vector2f scale, sf::Angle angle)
    : position_{std::move(position)}
    , scale_{std::move(scale)}
    , angle_{std::move(angle)} {
}

const sf::Vector2f& TransformComponent::get_position() const {
    return this->position_;
}

sf::Angle TransformComponent::get_rotation() const {
    return this->angle_;
}

const sf::Vector2f& TransformComponent::get_scale() const {
    return this->scale_;
}

sf::Vector2f TransformComponent::get_origin() const {
    return this->origin_;
}

void TransformComponent::set_origin(const sf::Vector2f& origin) {
    this->origin_ = origin;
}

void TransformComponent::set_position(sf::Vector2f position) {
    position_ = std::move(position);
}

void TransformComponent::set_rotation(sf::Angle angle) {
    this->angle_ = angle;
}

void TransformComponent::set_scale(sf::Vector2f scale) {
    this->scale_ = std::move(scale);
}

void TransformComponent::translate(const sf::Vector2f& offset) {
    this->position_ += offset;
}
} // namespace engine::component