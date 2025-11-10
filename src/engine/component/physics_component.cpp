#include "physics_component.hpp"
#include "physics_engine.hpp"
#include "transform_component.hpp"
#include "game_object.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
PhysicsComponent::PhysicsComponent(engine::object::GameObject* owner, engine::physics::PhysicsEngine* physics_engine, bool use_gravity, float mass) 
    : Component(owner)
    , physics_engine_obs_{physics_engine}
    , mass_{mass >= 0.f ? mass : 1.f}
    , use_gravity_{use_gravity} {
    if (!physics_engine_obs_) {
        spdlog::error("PhysicsComponent构造函数中，PhysicsEngine指针不能为nullptr！");
    }
    transform_obs_ = owner_->get_component<TransformComponent>();
    if (!transform_obs_) {
        spdlog::warn("物理组件初始化时，同一GameObject上没有找到TransformComponent组件。");
    }
    // 注册到PhysicsEngine
    physics_engine_obs_->register_component(this);
    spdlog::trace("物理组件初始化完成。");

    spdlog::trace("物理组件创建完成，质量: {}, 使用重力: {}", mass_, use_gravity_);
}

PhysicsComponent::~PhysicsComponent() {
    physics_engine_obs_->unregister_component(this);
    spdlog::trace("物理组件清理完成。");
}

void PhysicsComponent::add_force(const sf::Vector2f& force) {
    if (enabled_) force_ += force;
}

void PhysicsComponent::clear_force() {
    force_ = {0.0f, 0.0f};
}

const sf::Vector2f& PhysicsComponent::get_force() const {
    return force_;
}

float PhysicsComponent::get_mass() const {
    return mass_;
}

bool PhysicsComponent::is_enabled() const {
    return enabled_;
}

bool PhysicsComponent::is_use_gravity() const {
    return use_gravity_;
}

void PhysicsComponent::set_enabled(bool enabled) {
    enabled_ = enabled;
}

void PhysicsComponent::set_mass(float mass) {
    mass_ = (mass >= 0.0f) ? mass : 1.0f;
}

void PhysicsComponent::set_use_gravity(bool use_gravity) {
    use_gravity_ = use_gravity;
}

void PhysicsComponent::set_velocity(sf::Vector2f velocity) {
    velocity_ = std::move(velocity);
}

const sf::Vector2f& PhysicsComponent::get_velocity() const {
    return velocity_;
}

TransformComponent* PhysicsComponent::get_transform() const {
    return transform_obs_;
}
} // namespace engine::component