#pragma once
#include "player_state.hpp"

namespace game::component::state {
class FallState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    FallState(PlayerComponent* player_component);
    ~FallState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta_time, engine::core::Context& context) override;
};
} // namespace game::component::state