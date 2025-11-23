#pragma once
#include "player_state.hpp"

namespace game::component::state {
class HurtState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    HurtState(PlayerComponent* player_component);
    ~HurtState() override = default;

private:
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta, engine::core::Context& context) override;

    sf::Time stunned_timer_ = sf::Time::Zero;
};
} // namespace game::component::state