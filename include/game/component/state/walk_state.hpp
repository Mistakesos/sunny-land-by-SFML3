#pragma once
#include "player_state.hpp"

namespace game::component::state {
class WalkState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    WalkState(PlayerComponent* player_component);
    ~WalkState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta_time, engine::core::Context& context) override;
};
} // namespace game::component::state