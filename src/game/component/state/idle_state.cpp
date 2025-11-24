#include "idle_state.hpp"
#include "walk_state.hpp"
#include "jump_state.hpp"
#include "fall_state.hpp"
#include "context.hpp"
#include "input_manager.hpp"
#include "physics_component.hpp"
#include "player_component.hpp"

namespace game::component::state {
IdleState::IdleState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    // 播放动画
    play_animation("idle");
    
    spdlog::debug("PlayerComponent 进入 IdleState");
}

void IdleState::handle_input(engine::core::Context& context) {
    auto input_manager = context.get_input_manager();
    
    // 如果按下了左右移动键，则切换到 WalkState
    if (input_manager.is_action_held(Action::MoveLeft) || input_manager.is_action_held(Action::MoveRight)) {
        transition<WalkState>();
    }

    // 如果按下“jump”则切换到 JumpState
    if (input_manager.is_action_pressed(Action::Jump)) {
        transition<JumpState>();
    }
}

void IdleState::update(sf::Time, engine::core::Context&) {
    // 应用摩擦力(水平方向)
    auto physics_component = player_component_obs_->get_physics_component();
    auto friction_factor = player_component_obs_->get_friction_factor();
    physics_component->velocity_.x *= friction_factor;

    // 如果离地，则切换到 FallState
    if (!physics_component->has_collided_below()) {
        transition<FallState>();
    }
}
} // namespace game::component::state