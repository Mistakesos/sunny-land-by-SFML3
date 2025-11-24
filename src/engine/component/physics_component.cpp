#include "physics_component.hpp"
#include "physics_engine.hpp"
#include "transform_component.hpp"
#include "game_object.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
PhysicsComponent::PhysicsComponent(engine::object::GameObject* owner, engine::physics::PhysicsEngine* physics_engine, bool use_gravity, float mass) 
    : Component{owner}
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
} // namespace engine::component