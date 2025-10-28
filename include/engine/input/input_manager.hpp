#pragma once
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <vector>

namespace sf {
    class RenderWindow;
    class KeyBoard;
} // namespace sf

namespace engine::core {
    class Config;
} // namespace engine::core

namespace engine::input {
enum class ActionState {
    Inactive,               ///< @brief 动作未激活
    PressedThisFrame,       ///< @brief 动作在本帧刚刚按下
    HeldDown,               ///< @brief 动作被持续按下
    ReleasedThisFrame       ///< @brief 动作在本帧被刚刚释放
};

class InputManager final {
public:
    
private:
    sf::RenderWindow* window_obs_ = nullptr;
    // std::unordered_map<
};
}