#include "updown_behavior.hpp"
#include "ai_component.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include "animation_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {
UpDownBehavior::UpDownBehavior(AIComponent* ai_component, float min_y, float max_y, float speed)
    : AIBehavior{ai_component}
    , patrol_min_y_{min_y}
    , patrol_max_y_{max_y}
    , move_speed_{speed} {
    if (patrol_min_y_ >= patrol_max_y_) {
        spdlog::error("UpDownBehavior：min_y ({}) 应小于 max_y ({})。行为可能不正确。", min_y, max_y);
        patrol_min_y_ = patrol_max_y_;  // 修正为相等，避免逻辑错误
    }

    // 播放动画 (进行 up-down 行为的对象应该有 'fly' 动画)
    if (auto* animation_component = ai_component_obs_->get_animation_component(); animation_component) {
        animation_component->play_animation("fly");
    }

    // 禁用重力
    if (auto* physics_component = ai_component_obs_->get_physics_component(); physics_component) {
        physics_component->set_use_gravity(false);
    }
}

void UpDownBehavior::update(sf::Time) {
    // 获取必要的组件
    auto* physics_component = ai_component_obs_->get_physics_component();
    auto* transform_component = ai_component_obs_->get_transform_component();
    if (!physics_component || !transform_component) {
        spdlog::error("UpdownBehavior：缺少必要的组件，无法执行巡逻行为。");
        return;
    }

    // --- 检查碰撞和边界 ---
    auto current_y = transform_component->get_position().y;

    // 到达上边界或碰到上方障碍，向下移动
    if (physics_component->has_collided_above() || current_y <= patrol_min_y_) {
        physics_component->velocity_.y = move_speed_;
        moving_down_ = true;
    // 到达下边界或碰到下方障碍，向上移动
    } else if (physics_component->has_collided_below() || current_y >= patrol_max_y_) {
        physics_component->velocity_.y = -move_speed_;
        moving_down_ = false;
    }
    /* 不需要翻转精灵图 */
}
} // namespace game::component::ai