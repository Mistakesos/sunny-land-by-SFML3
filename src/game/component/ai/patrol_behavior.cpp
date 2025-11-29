#include "patrol_behavior.hpp"
#include "ai_component.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include "collider_component.hpp"
#include "animation_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {
PatrolBehavior::PatrolBehavior(AIComponent* ai_component, float min_x, float max_x, float speed)
    : AIBehavior{ai_component}
    , patrol_min_x_{min_x}
    , patrol_max_x_{max_x}
    , move_speed_{speed} {
    if (patrol_min_x_ >= patrol_max_x_) {
        spdlog::error("PatrolBehavior：min_x ({}) 应小于 max_x ({})。行为可能不正确。", min_x, max_x);
        patrol_min_x_ = patrol_max_x_;  // 修正为相等，避免逻辑错误
    }

    // 播放动画 (进行 patrol 行为的对象应该有 'walk' 动画)
    if (auto* animation_component = ai_component->get_animation_component(); animation_component) {
        animation_component->play_animation("walk");
    }
}

void PatrolBehavior::update(sf::Time) {
    // 获取必要的组件
    auto* physics_component = ai_component_obs_->get_physics_component();
    auto* collider_component = ai_component_obs_->get_collider_component();
    auto* transform_component = ai_component_obs_->get_transform_component();
    if (!physics_component || !transform_component) {
        spdlog::error("PatrolBehavior：缺少必要的组件，无法执行巡逻行为。");
        return;
    }

    // --- 检查碰撞和边界 ---
    auto current_x = transform_component->get_position().x;

    // 撞右墙或到达设定目标则转向左
    if (physics_component->has_collided_right() || current_x >= patrol_max_x_) {
        physics_component->velocity_.x = -move_speed_;
        moving_right_ = false;
    // 撞墙左或到达设定目标则转向右
    }else if (physics_component->has_collided_left() || current_x <= patrol_min_x_) {
        physics_component->velocity_.x = move_speed_;
        moving_right_ = true;
    }

    // 更新精灵翻转(向左移动时，不翻转)
    const auto& size = collider_component->get_world_aabb().size;
    const auto& scale = transform_component->get_scale();
    transform_component->set_origin({size.x / 2.f, size.y});
    if (moving_right_) {
        transform_component->set_scale({-std::abs(scale.x), scale.y});
    } else {
        transform_component->set_scale({std::abs(scale.x), scale.y});
    }
}
} // namespace game::component::ai