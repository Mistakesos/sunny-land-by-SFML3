#include "player_state.hpp"
#include "game_object.hpp"
#include "player_component.hpp"
#include <spdlog/spdlog.h>

namespace game::component::state {
PlayerState::PlayerState(PlayerComponent* player_component)
    : player_component_obs_{player_component} {
}
}