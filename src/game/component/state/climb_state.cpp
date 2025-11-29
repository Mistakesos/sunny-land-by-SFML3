#include "climb_state.hpp"
#include "jump_state.hpp"
#include "idle_state.hpp"
#include "walk_state.hpp"
#include "fall_state.hpp"
#include "player_component.hpp"
#include "context.hpp"
#include "input_manager.hpp"
#include "physics_component.hpp"
#include "sprite_component.hpp"
#include "animation_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::state {
ClimbState::ClimbState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    spdlog::debug("进入攀爬状态");
    play_animation("climb");
    if (auto* physics = player_component_obs_->get_physics_component(); physics) {
        physics->set_use_gravity(false); // 禁用重力
    }
}

ClimbState::~ClimbState() {
    spdlog::debug("退出攀爬状态");

    if (auto* physics = player_component_obs_->get_physics_component(); physics) {
        physics->set_use_gravity(true); // 重新启用重力
    }
}

void ClimbState::handle_input(engine::core::Context& context) {
    auto input_manager = context.get_input_manager();
    auto physics_component = player_component_obs_->get_physics_component();
    auto animation_component = player_component_obs_->get_animation_component();

    // --- 攀爬状态下，按键则移动，不按键则静止 ---
    auto is_up = input_manager.is_action_held(Action::MoveUp);
    auto is_down = input_manager.is_action_held(Action::MoveDown);
    auto is_left = input_manager.is_action_held(Action::MoveLeft);
    auto is_right = input_manager.is_action_held(Action::MoveRight);
    auto speed = player_component_obs_->get_climb_speed();

    physics_component->velocity_.y = is_up ? -speed :   // 三目运算符嵌套，自左向右执行
                                     is_down ? speed : 0.f;
    physics_component->velocity_.x = is_left ? -speed :
                                     is_right ? speed : 0.f;

    // --- 根据是否有按键决定动画播放情况 ---
    (is_up || is_down || is_left || is_right)
        ? animation_component->resume_animation()    // 有按键则恢复动画播放
        : animation_component->stop_animation();     // 无按键则停止动画播放
    
    // 按跳跃键主动离开攀爬状态
    if (input_manager.is_action_pressed(Action::Jump)) {
        transition<JumpState>();
    }
}
void ClimbState::update(sf::Time, engine::core::Context&) {
    auto physics_component = player_component_obs_->get_physics_component();
    // 如果着地，则切换到 IdleState
    if (physics_component->has_collided_below()) {
        transition<IdleState>();
    }
    // 如果离开梯子区域，则切换到 FallState（能走到这里 说明非着地状态）
    if (!physics_component->has_collided_ladder()) {
        transition<FallState>();
    }
}
} // namespace game::component::state