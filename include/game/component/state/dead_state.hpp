#pragma once
#include "player_state.hpp"

namespace game::component::state {
class DeadState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    DeadState(PlayerComponent* player_component);
    ~DeadState() override = default;

private:
    void handle_input(engine::core::Context&) override;
    void update(sf::Time, engine::core::Context&) override;
};
} // namespace game::component::state