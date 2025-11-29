#pragma once
#include "player_state.hpp"

namespace game::component::state {
class ClimbState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    ClimbState(PlayerComponent* player_component);
    ~ClimbState() override;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta, engine::core::Context& context) override;
};
} // namespace game::component::state