#include "ai_component.hpp"
#include "ai_behavior.hpp"
#include "game_object.hpp"
#include "transform_component.hpp"
#include "physics_component.hpp"
#include "collider_component.hpp"
#include "animation_component.hpp"
#include "health_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component {
AIComponent::AIComponent(engine::object::GameObject* owner)
    : Component{owner} {
    // 获取并缓存必要的组件指针
    transform_component_obs_ = owner_->get_component<engine::component::TransformComponent>();
    physics_component_obs_ = owner_->get_component<engine::component::PhysicsComponent>();
    collider_component_obs_ = owner_->get_component<engine::component::ColliderComponent>();
    animation_component_obs_ = owner_->get_component<engine::component::AnimationComponent>();

    // 检查是否所有必需的组件都存在
    if (!transform_component_obs_ || !physics_component_obs_ || !collider_component_obs_ || !animation_component_obs_) {
        spdlog::error("GameObject '{}' 上的 AIComponent 缺少必需的组件", owner_->get_name());
    }
}

void AIComponent::update(sf::Time delta, engine::core::Context&) {
    // 将更新委托给当前的行为策略
    if (current_behavior_) {
        current_behavior_->update(delta);
    } else {
        spdlog::warn("GameObject '{}' 上的 AIComponent 没有设置行为。", owner_->get_name());
    }
}

void AIComponent::set_behavior(std::unique_ptr<ai::AIBehavior> behavior) {
    current_behavior_ = std::move(behavior);
    spdlog::debug("GameObject '{}' 上的 AIComponent 设置了新的行为。", owner_->get_name());
}

bool AIComponent::take_damage(int damage) {
    bool success = false;
    if (auto* health_component = get_owner()->get_component<engine::component::HealthComponent>(); health_component) {
        success = health_component->take_damage(damage);
        // TODO: 可以设置受伤/死亡后的行为
    }
    return success;
}

bool AIComponent::is_alive() const {
    if (auto* health_component = get_owner()->get_component<engine::component::HealthComponent>(); health_component) {
        return health_component->is_alive();
    }
    return true;    // 如果没有生命组件，默认返回存活状态
}
} // namespace game::component
