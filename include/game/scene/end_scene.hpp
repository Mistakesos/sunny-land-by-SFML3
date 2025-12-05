#pragma once
#include "scene.hpp"
#include <memory>
#include <string>

// 前置声明
namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::scene {
    class SceneManager;
} // namespace engine::scene

namespace game::data {
    class SessionData;
} // namespace game::data

namespace engine::ui {
    class UIManager;
    class Label;
    class Button;
} // namespace engine::ui

namespace engine::input {
    class InputManager;
} // namespace engine::input

namespace engine::render {
    class Renderer;
    class Camera;
} // namespace engine::render

namespace game::scene {
/**
 * @class EndScene
 * @brief 显示游戏结束（胜利或失败）信息的叠加场景。
 *
 * 提供重新开始或返回主菜单的选项。
 */
class EndScene final : public engine::scene::Scene {
private:
    std::shared_ptr<game::data::SessionData> session_data_;

public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param scene_manager 场景管理器
     * @param session_data 指向游戏数据状态的共享指针
     */
    EndScene(engine::core::Context& context
           , engine::scene::SceneManager& scene_manager
           , std::shared_ptr<game::data::SessionData> session_data);

    ~EndScene() override = default;

    // 禁止拷贝和移动
    EndScene(const EndScene&) = delete;
    EndScene& operator=(const EndScene&) = delete;
    EndScene(EndScene&&) = delete;
    EndScene& operator=(EndScene&&) = delete;

private:
    // 用于创建UI和按钮回调的私有辅助方法
    void create_ui();
    void on_back_click();
    void on_restart_click();};
} // namespace game::scene