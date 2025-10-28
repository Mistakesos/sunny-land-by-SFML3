#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>        // 官方提供的前向声明头文件

namespace engine::core {
enum class Action {
    None,
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Jump,
    Attack,
    Pause
};
/**
 * @brief 管理应用程序的配置设置
 * 提供配置项的默认值，且支持从 JSON 文件加载/保存配置
 * 如果加载失败或文件不存在，将使用默认值
 */
class Config final {
public:
    explicit Config(const std::string& filepath);   ///< @brief 构造函数，制定配置文件路径

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&) = delete;
    Config& operator=(Config&&) = delete;

    bool load_from_file(const std::string& filepath);               ///< @brief 从指定的json文件加载配置，成功返回true
    [[nodiscard]] bool save_to_file(const std::string& filepath);   ///< @brief 将当前配置保存到指定的json文件，成功返回true（不应忽略返回值）

private:
    void from_json(const nlohmann::json& json);
    nlohmann::ordered_json to_json() const;

public:
    // --- 默认配置 ---
    std::string window_title_ = "Sunny Land";
    sf::Vector2u window_size_ = {1280u, 720u};
    bool window_resizable_ = true;

    // 图形设置
    bool vsync_enabled_ = false;                    ///< @brief 垂直同步（默认关闭）

    // 性能设置
    unsigned int target_fps_ = 60;                  ///< @brief 目标FPS，0表示无限制

    // 音频设置
    float music_volume_ = 0.5f;
    float sound_volume_ = 0.5f;

    // 存储动作名称到 sfml scancode/button 的名称列表映射
    using Key = sf::Keyboard::Scancode;
    using Button = sf::Mouse::Button;
    std::unordered_map<Action, std::vector<sf::Keyboard::Scancode>> keyboard_input_mappings_ = {
        {Action::MoveLeft, {Key::A, Key::Left}},
        {Action::MoveRight, {Key::D, Key::Right}},
        {Action::MoveUp, {Key::W, Key::Up}},
        {Action::MoveDown, {Key::S, Key::Down}},
        {Action::Jump, {Key::J, Key::Space}},
        {Action::Attack, {Key::K}},
        {Action::Pause, {Key::P, Key::Escape}}
    };
    std::unordered_map<Action, std::vector<sf::Mouse::Button>> mouse_input_mappings_ = {
        {Action::Attack, {Button::Left}}
    };
};
} // namespace engine::core