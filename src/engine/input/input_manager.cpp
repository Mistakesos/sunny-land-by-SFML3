#include "input_manager.hpp"
#include "config.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <optional>

namespace engine::input {
InputManager::InputManager(sf::RenderWindow* window, const engine::core::Config* config) 
    : window_obs_{window}
    , action_to_input_copy_{config->action_to_input_} {
    for (const auto& [action, inputs] : action_to_input_copy_) {
        action_state_map_.emplace(action, ActionState::Inactive);
    }
}

void InputManager::update() {
    for (auto& [action, state] : action_state_map_) {
        if (state == ActionState::PressedThisFrame) {
            state = ActionState::HeldDown;
        } else if (state == ActionState::ReleasedThisFrame) {
            state = ActionState::Inactive;
        }
    }

    process_event();

    for (auto& [action, inputs] : action_to_input_copy_) {
        bool pressed = false;

        for (const auto& input : inputs) {
            std::visit([&pressed](auto&& input_value) {
                using T = std::decay_t<decltype(input_value)>;

                if constexpr (std::is_same_v<T, sf::Keyboard::Scancode>) {
                    if (sf::Keyboard::isKeyPressed(input_value)) {
                        pressed = true;
                    }
                } else if constexpr (std::is_same_v<T, sf::Mouse::Button>) {
                    if (sf::Mouse::isButtonPressed(input_value)) {
                        pressed = true;
                    }
                }
            }, input);

            if (pressed) break;
        }

        ActionState prev = action_state_map_.at(action);
        if (pressed) {
            if (prev == ActionState::Inactive || prev == ActionState::ReleasedThisFrame)
                action_state_map_.at(action) = ActionState::PressedThisFrame;
            else
                action_state_map_.at(action) = ActionState::HeldDown;
        } else {
            if (prev == ActionState::HeldDown || prev == ActionState::PressedThisFrame)
                action_state_map_.at(action) = ActionState::ReleasedThisFrame;
            else
                action_state_map_.at(action) = ActionState::Inactive;
        }
    }
}

bool InputManager::is_action_held(Action action) const {
    if (auto it = action_state_map_.find(action); it != action_state_map_.end()) {
        return it->second == ActionState::PressedThisFrame || it->second == ActionState::HeldDown;
    }
    return false;
}

bool InputManager::is_action_pressed(Action action) const {
    if (auto it = action_state_map_.find(action); it != action_state_map_.end()) {
        return it->second == ActionState::PressedThisFrame;
    }
    return false;
}

bool InputManager::is_action_released(Action action) const {
    if (auto it = action_state_map_.find(action); it != action_state_map_.end()) {
        return it->second == ActionState::ReleasedThisFrame;
    }
    return false;
}

sf::Vector2i InputManager::get_mouse_position() const {
    return sf::Mouse::getPosition();
}

sf::Vector2i InputManager::get_mouse_logical_position() const {
    return sf::Mouse::getPosition(*window_obs_);
}

void InputManager::process_event() {
    while (std::optional event = window_obs_->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_obs_->close();
        }
    }
}
} // namespace engine::input