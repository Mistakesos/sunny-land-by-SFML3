#include "player_state.hpp"
#include "game_object.hpp"
#include "player_component.hpp"
#include "animation_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::state {
PlayerState::PlayerState(PlayerComponent* player_component)
    : player_component_obs_{player_component} {
}

void PlayerState::play_animation(const std::string& animation_name) {
    if (!player_component_obs_) {
        spdlog::error("PlayerState 没有关联的 PlayerComponent，无法播放动画 '{}'", animation_name);
        return;
    }
    
    auto animation_component = player_component_obs_->get_animation_component();
    if (!animation_component) {
        spdlog::error("PlayerComponent '{}' 没有 AnimationComponent，无法播放动画 '{}'", 
                      player_component_obs_ ->get_owner()->get_name(), animation_name);
        return;
    }

    animation_component->play_animation(animation_name);
}
} // namespace game::component::state