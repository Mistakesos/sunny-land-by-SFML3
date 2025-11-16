#include "collider_component.hpp"
#include "transform_component.hpp"
#include "game_object.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
ColliderComponent::ColliderComponent(engine::object::GameObject* owner, std::unique_ptr<engine::physics::Collider> collider, bool is_trigger, bool is_active)
    : Component{owner}
    , collider_{std::move(collider)}
    , is_trigger_{is_trigger}
    , is_active_{is_active} {
    transform_obs_ = owner_->get_component<TransformComponent>();
    if (!transform_obs_) {
        spdlog::error("ColliderComponent 需要一个在同一个 GameObject 上的 TransformComponent！");
        return;
    }
}

sf::FloatRect ColliderComponent::get_world_aabb() const {
    if (!transform_obs_ || !collider_) {
        return sf::FloatRect{{0.f, 0.f}, {0.f, 0.f}};
    }
    const sf::Vector2f position = transform_obs_->get_position() - transform_obs_->get_origin();
    const sf::Vector2f size = collider_->get_aabb_size().componentWiseMul(transform_obs_->get_scale());
    return {position, size};
}
} // namespace engine::component