#include "physics_engine.hpp"
#include "collision.hpp"
#include "game_object.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include "collider_component.hpp"
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
    // 每次开始时先清空碰撞对容器
    collision_pairs_.clear();
    
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

        // 处理对象间碰撞
        check_object_collisions();
    }
}

void PhysicsEngine::check_object_collisions() {
    // 两层循环遍历所有包含物理组件的 GameObject
    for (size_t i = 0; i < components_.size(); ++i) {
        auto* pc_a = components_[i];
        if (!pc_a || !pc_a->is_enabled()) continue;
        auto* obj_a = pc_a->get_owner();
        if (!obj_a) continue;
        auto* cc_a = obj_a->get_component<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->is_active()) continue;

        for (size_t j = i + 1; j < components_.size(); ++j) {
            auto* pc_b = components_[j];
            if (!pc_b || !pc_b->is_enabled()) continue;
            auto* obj_b = pc_b->get_owner();
            if (!obj_b) continue;
            auto* cc_b = obj_b->get_component<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->is_active()) continue;
            /* --- 通过保护性测试后，正式执行逻辑 --- */

            if (collision::check_collision(*cc_a, *cc_b)) {
                // TODO: 不是所有碰撞都插入collision_pairs 未来添加过滤条件
                // 记录碰撞对
                collision_pairs_.emplace_back(obj_a, obj_b);
            }
        }
    }
}
} // engine::physics