#include "hurt_state.hpp"
#include "idle_state.hpp"
#include "walk_state.hpp"
#include "fall_state.hpp"
#include "player_component.hpp"
#include "transform_component.hpp"
#include "physics_component.hpp"
#include "audio_component.hpp"
#include <SFML/System/Vector2.hpp>

namespace game::component::state {
HurtState::HurtState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    play_animation("hurt");  // 播放受伤动画

    if (auto* audio_component = player_component_obs_->get_audio_component(); audio_component) {
        audio_component->play_sound("hurt");  // 播放受伤音效
    }

    // --- 造成击退效果 ---
    auto physics_component = player_component_obs_->get_physics_component();
    auto transform_component = player_component_obs_->get_transform_component();
    auto knockback_velocity = sf::Vector2f(-100.f, -150.f);      // 默认左上方击退效果
    // 根据当前精灵的朝向状态决定是否改成右上方（根据transform组件判断）
    if (transform_component->get_scale() == sf::Vector2f(-1.f, 1.f)) {
        knockback_velocity.x = -knockback_velocity.x;   // 变成向右
    }
    physics_component->velocity_ = knockback_velocity;  // 设置击退速度

    spdlog::debug("PlayerComponent 进入 HurtState");
}

void HurtState::handle_input(engine::core::Context&) {
    // 硬直期间不能进行任何操控
    return;
}

void HurtState::update(sf::Time delta, engine::core::Context&) {
    stunned_timer_ += delta;
    // --- 两种情况离开受伤（硬直）状态：---
    // 1. 落地
    auto physics_component = player_component_obs_->get_physics_component();
    if (physics_component->has_collided_below()) {
        if (std::abs(physics_component->velocity_.x) < 1.f) {
            transition<IdleState>();
        } else {
            transition<WalkState>();
        }
    }
    // 2. 硬直时间结束(能走到这里说明没有落地，直接切换到 FallState)
    if (stunned_timer_ > player_component_obs_->get_stunned_duration()){
        stunned_timer_ = sf::Time::Zero;    // 重置硬直计时器
        transition<FallState>();            // 切换到下落状态
    }
    return;
}
} // namespace game::component::state