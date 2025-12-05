#include "jump_behavior.hpp"
#include "ai_component.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include "collider_component.hpp"
#include "animation_component.hpp"
#include "audio_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {
JumpBehavior::JumpBehavior(AIComponent* ai_component
                         , float min_x
                         , float max_x
                         , sf::Vector2f jump_vel
                         , sf::Time jump_interval) 
    : AIBehavior{ai_component}
    , patrol_min_x_{min_x}
    , patrol_max_x_{max_x}
    , jump_vel_{std::move(jump_vel)}
    , jump_interval_{std::move(jump_interval)} {
    if (patrol_min_x_ >= patrol_max_x_) {    // 确保巡逻范围是有效的
        spdlog::error("JumpBehavior: min_x ({}) 应小于 max_x ({})。行为可能不正确。", min_x, max_x);
        patrol_min_x_ = patrol_max_x_;
    }
    if (jump_interval_ <= sf::Time::Zero) {  // 确保跳跃间隔是正数
        spdlog::error("JumpBehavior: jump_interval ({}) 应为正数。已设置为 2.f。", jump_interval.asSeconds());
        jump_interval_ = sf::seconds(2.f);
    }
    if (jump_vel_.y > 0.f) {                 // 确保垂直跳跃速度是负数（向上）
         spdlog::error("JumpBehavior: 垂直跳跃速度 ({}) 应为负数（向上）。已取相反数。", jump_vel_.y);
         jump_vel_.y = -jump_vel_.y;
    }
}

void JumpBehavior::update(sf::Time delta_time) {
    // 获取必要的组件
    auto* physics_component = ai_component_obs_->get_physics_component();
    auto* transform_component = ai_component_obs_->get_transform_component();
    auto* animation_component = ai_component_obs_->get_animation_component();
    auto* audio_component = ai_component_obs_->get_audio_component();

    if (!physics_component || !transform_component || !animation_component) {
        spdlog::error("JumpBehavior：缺少必要的组件，无法执行跳跃行为。");
        return;
    }

    auto is_on_ground = physics_component->has_collided_below();      // 着地标志
    if (is_on_ground) {    // 如果在地面上
        if (audio_component && jump_timer_ == sf::Time::Zero) {      // 刚刚落地时（进入idle状态），如果有音频组件，播放音效
            audio_component->play_sound("cry", true);       // 使用空间音频
        }

        jump_timer_ += delta_time;              // 增加跳跃计时器
        physics_component->velocity_.x = 0.f;   // 停止水平移动（否则会有惯性）

        if (jump_timer_ >= jump_interval_) {    // 时间到，准备跳跃
            jump_timer_ = sf::Time::Zero;       // 重置计时器
            
            // --- 检查是否需要更新跳跃方向 ---
            auto current_x = transform_component->get_position().x;
            // 如果右边超限或者撞墙，向左跳
            if (jumping_right_ && (physics_component->has_collided_right() || current_x >= patrol_max_x_)) {
                jumping_right_ = false;
                // 如果左边超限或者撞墙，向右跳
            } else if (!jumping_right_ && (physics_component->has_collided_left() || current_x <= patrol_min_x_)) {
                jumping_right_ = true;
            }
            auto jump_vel_x = jumping_right_ ? jump_vel_.x : -jump_vel_.x;  // 确定水平跳跃方向
            physics_component->velocity_= {jump_vel_x, jump_vel_.y};        // 设置速度
            animation_component->play_animation("jump");                    // 播放跳跃动画

            // 更新精灵翻转
            const auto& scale = transform_component->get_scale();
            if (jumping_right_) {
                transform_component->set_scale({-std::abs(scale.x), scale.y});
            } else {
                transform_component->set_scale({std::abs(scale.x), scale.y});
            }
        } else {    // 还在地面等待
             animation_component->play_animation("idle");
        }
    } else {        // 在空中, 根据垂直速度判断是上升(jump)还是下落(fall)
        if (physics_component->get_velocity().y < 0.f) {
            animation_component->play_animation("jump");
        } else {
            animation_component->play_animation("fall");
        }
    }
}
} // namespace game::component::ai