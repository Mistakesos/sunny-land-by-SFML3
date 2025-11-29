#include "idle_state.hpp"
#include "walk_state.hpp"
#include "jump_state.hpp"
#include "fall_state.hpp"
#include "climb_state.hpp"
#include "context.hpp"
#include "input_manager.hpp"
#include "transform_component.hpp"
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
    auto physics_component = player_component_obs_->get_physics_component();
    
    // 如果按下上键，且与梯子重合，则切换到 ClimbState
    if (physics_component->has_collided_ladder() && (input_manager.is_action_held(Action::MoveUp))) {
        transition<ClimbState>();
    }

    // 如果按下“move_down”且在梯子顶层，则切换到 ClimbState
    if (physics_component->is_on_top_ladder() && input_manager.is_action_held(Action::MoveDown)) {
        // 需要向下移动一点，确保下一帧能与梯子碰撞（否则会切换回FallState）
        player_component_obs_->get_transform_component()->translate({0.f, 15.f});
        transition<ClimbState>();
    }

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
    if (!player_component_obs_->is_on_ground()) {
        transition<FallState>();
    }
}
} // namespace game::component::state