#pragma once
#include "player_state.hpp"

namespace game::component::state {
class DeadState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    DeadState(PlayerComponent* player_component);
    ~DeadState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta_time, engine::core::Context& context) override;
};
} // namespace game::component::state