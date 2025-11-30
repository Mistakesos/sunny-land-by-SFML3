#include "dead_state.hpp"
#include "player_component.hpp"
#include "physics_component.hpp"
#include "collider_component.hpp"
#include "audio_component.hpp"
#include <SFML/System/Vector2.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {
DeadState::DeadState(PlayerComponent* player_component)
    : PlayerState{player_component} {
    spdlog::debug("玩家进入死亡状态。");
    
    play_animation("hurt");  // 播放死亡(受伤)动画

    if (auto* audio_component = player_component_obs_->get_audio_component(); audio_component) {
        audio_component->play_sound("dead");  // 播放死亡音效
    }
    
    // 应用击退力（只向上）
    auto physics_component = player_component_obs_->get_physics_component();
    physics_component->velocity_ = sf::Vector2f(0.f, -200.f);  // 向上击退

    // 禁用碰撞(自动掉出屏幕)
    auto collider_component = player_component_obs_->get_owner()->get_component<engine::component::ColliderComponent>();
    if (collider_component) {
        collider_component->set_active(false);
    }
}

void DeadState::handle_input(engine::core::Context&) {
    // 死亡状态下不处理输入
    return;
}

void DeadState::update(sf::Time, engine::core::Context&) {
    // 死亡状态下不更新状态
    return;
}
} // namespace game::component::state