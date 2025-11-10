#include "physics_engine.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::physics {
void PhysicsEngine::register_component(engine::component::PhysicsComponent* component) {
    components_.push_back(component);
    spdlog::trace("物理组件注册完成");
}

void PhysicsEngine::unregister_component(engine::component::PhysicsComponent* component) {
    // 使用 remove-erase 方法安全的移除指针
    auto it = std::remove(components_.begin(), components_.end(), component);
    components_.erase(it, components_.end());
    spdlog::trace("物理组件注销完成");
}

void PhysicsEngine::update(sf::Time delta) {
    for (auto* pc : components_) {
        if (!pc || !pc->is_enabled()) {
            continue;
        }

        // 应用重力
        if (pc->is_use_gravity()) {
            pc->add_force(gravity_ * pc->get_mass());
        }
        /* 还能添加其他力影响，比如风力、摩擦力、目前不考虑 */
        // 更新速度：v += a * dt, 其中 a = F / m
        pc->velocity_ += (pc->get_force() / pc->get_mass()) * delta.asSeconds();
        pc->clear_force();  // 清除当前帧的力

        // 更新位置：S += v * dt
        auto* tc = pc->get_transform();
        if (tc) {
            tc->translate(pc->velocity_ * delta.asSeconds());
        }

        // 限制最大速度
        pc->velocity_.x = std::clamp(pc->velocity_.x, -max_speed_.x, max_speed_.x);
        pc->velocity_.y = std::clamp(pc->velocity_.y, -max_speed_.y, max_speed_.y);
    }
}

} // engine::physics