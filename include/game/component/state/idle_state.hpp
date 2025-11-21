#pragma once
#include "player_state.hpp"

namespace game::component::state {
class IdleState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    IdleState(PlayerComponent* player_component);
    ~IdleState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta_time, engine::core::Context& context) override;
};
} // namespace game::component::state