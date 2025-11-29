#include "jump_state.hpp"
#include "fall_state.hpp"
#include "context.hpp"
#include "input_manager.hpp"
#include "physics_component.hpp"
#include "player_component.hpp"
#include "transform_component.hpp"

namespace game::component::state {
JumpState::JumpState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    // 播放动画
    play_animation("jump");
    
    auto physics_component = player_component_obs_->get_physics_component();
    physics_component->velocity_.y = -player_component_obs_->get_jump_velocity();     // 向上跳跃
    spdlog::debug("PlayerComponent 进入 JumpState，设置初始垂直速度为: {}", physics_component->velocity_.y);
}

void JumpState::handle_input(engine::core::Context& context) {
    auto input_manager = context.get_input_manager();
    auto physics_component = player_component_obs_->get_physics_component();
    auto transform_component = player_component_obs_->get_transform_component();

    // 跳跃状态下可以左右移动
    const auto& scale = transform_component->get_scale();
    if (input_manager.is_action_held(Action::MoveLeft)) {
        if (physics_component->velocity_.x > 0.f) physics_component->velocity_.x = 0.f;
        physics_component->add_force({-player_component_obs_->get_move_force(), 0.f});
        transform_component->set_scale({-std::abs(scale.x), scale.y});
    } else if (input_manager.is_action_held(Action::MoveRight)) {
        if (physics_component->velocity_.x < 0.f) physics_component->velocity_.x = 0.f;
        physics_component->add_force({player_component_obs_->get_move_force(), 0.f});
        transform_component->set_scale({std::abs(scale.x), scale.y});
    }
}

void JumpState::update(sf::Time, engine::core::Context&) {
    // 限制最大速度(水平方向)
    auto physics_component = player_component_obs_->get_physics_component();
    auto max_speed = player_component_obs_->get_max_speed();
    physics_component->velocity_.x = std::clamp(physics_component->velocity_.x, -max_speed, max_speed);

    // 如果速度为正，切换到 FallState
    if (physics_component->velocity_.y >= 0.f) {
        transition<FallState>();
    }
}
} // namespace game::component::state