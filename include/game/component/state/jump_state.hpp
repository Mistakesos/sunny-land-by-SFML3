#pragma once
#include "player_state.hpp"

namespace game::component::state {
class JumpState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    JumpState(PlayerComponent* player_component);
    ~JumpState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time, engine::core::Context&) override;
};
} // namespace game::component::state