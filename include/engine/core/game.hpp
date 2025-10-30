#pragma once
#include <memory>

// 前向声明，减少头文件依赖，增加编译速度
namespace sf {
    class RenderWindow;
    class Time;
    class Event;
} // namespace sf

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::render {
    class Renderer;
    class Camera;
} // namespace engine::render

namespace engine::input {
    class InputManager;
}

namespace engine::core {
class Time;
class Config;

/**
 * @brief 主游戏类，初始化资源，管理游戏循环
 */
class Game final {
public:
    Game();
    ~Game();

    /**
     * @brief 运行游戏，构造，析构，不需要手动管理他
     */
    void run();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

private:
    void handle_event();
    void update(const sf::Time& delta);
    void render();

    // 测试用函数
    void test_resource_manager();
    void test_renderer();
    void test_camera();
    void test_input_manager();

    // 配置组件，优先加载，优先级最高
    std::unique_ptr<engine::core::Config> config_;

    // 游戏主窗口
    std::unique_ptr<sf::RenderWindow> window_;

    // 引擎组件
    std::unique_ptr<engine::core::Time> time_;                                  ///< @brief 时间组件
    std::unique_ptr<engine::resource::ResourceManager> resource_manager_;       ///< @brief 资源管理器组件
    std::unique_ptr<engine::input::InputManager> input_manager_;                ///< @brief 输入管理器组件
    std::unique_ptr<engine::render::Renderer> renderer_;                        ///< @brief 渲染器组件
    std::unique_ptr<engine::render::Camera> camera_;                            ///< @brief 摄像机组件
};
} // namespace engine::core