#include "transform_component.hpp"

namespace engine::component {
TransformComponent::TransformComponent(engine::object::GameObject* owner, sf::Vector2f position, sf::Vector2f scale, sf::Angle angle)
    : Component{owner}
    , position_{std::move(position)}
    , scale_{std::move(scale)}
    , angle_{std::move(angle)} {
}
} // namespace engine::component