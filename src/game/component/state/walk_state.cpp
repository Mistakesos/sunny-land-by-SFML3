#include "walk_state.hpp"
#include "idle_state.hpp"
#include "jump_state.hpp"
#include "fall_state.hpp"
#include "climb_state.hpp"
#include "sprite_component.hpp"
#include "transform_component.hpp"
#include "collider_component.hpp"
#include "animation_component.hpp"
#include "player_component.hpp"
#include "physics_component.hpp"
#include "input_manager.hpp"
#include "context.hpp"

namespace game::component::state {
WalkState::WalkState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    // 播放动画
    play_animation("walk");
    
    spdlog::debug("PlayerComponent 进入 WalkState");
}

void WalkState::handle_input(engine::core::Context& context) {
    auto input_manager = context.get_input_manager();
    auto physics_component = player_component_obs_->get_physics_component();
    auto transform_component = player_component_obs_->get_transform_component();

    // 如果按下上下键，且与梯子重合，则切换到 ClimbState
    if (physics_component->has_collided_ladder() &&
        (input_manager.is_action_held(Action::MoveUp))) {
        transition<ClimbState>();
    }

    // 如果按下“jump”则切换到 JumpState
    if (input_manager.is_action_pressed(Action::Jump)) {
        transition<JumpState>();
    }
    
    const auto& scale = transform_component->get_scale();
    // 步行状态可以左右移动
    if (input_manager.is_action_held(Action::MoveLeft)) {
        if (physics_component->velocity_.x > 0.f) {
            physics_component->velocity_.x = 0.f;           // 如果当前速度是向右的，则先减速到0 (增强操控手感)
        }
        // 添加向左的水平力
        physics_component->add_force({-player_component_obs_->get_move_force(), 0.f});
        transform_component->set_scale({-std::abs(scale.x), scale.y});                  // 向左移动时翻转
    } else if (input_manager.is_action_held(Action::MoveRight)) {
        if (physics_component->velocity_.x < 0.f) {
            physics_component->velocity_.x = 0.f;           // 如果当前速度是向左的，则先减速到0
        }
        // 添加向右的水平力
        physics_component->add_force({player_component_obs_->get_move_force(), 0.f});
        transform_component->set_scale({std::abs(scale.x), scale.y});         // 向右移动时重置翻转
    } else {
        // 如果没有按下左右移动键，则切换到 IdleState
        transition<IdleState>();
    }
}

void WalkState::update(sf::Time, engine::core::Context&) {
    // 限制最大速度
    auto physics_component = player_component_obs_->get_physics_component();
    auto max_speed = player_component_obs_->get_max_speed();
    physics_component->velocity_.x = std::clamp(physics_component->velocity_.x, -max_speed, max_speed);

    // 如果离地，则切换到 FallState
    if (!player_component_obs_->is_on_ground()) {
        transition<FallState>();
    }
}
} // namespace game::component::state