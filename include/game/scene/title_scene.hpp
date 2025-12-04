#pragma once
#include "scene.hpp"
#include <memory>

namespace engine::object {
    class GameObject;
} // namespace engine::object

namespace engine::scene {
    class SceneManager;
} // namespace engine::scene

namespace game::data {
    class SessionData;
} // namespace game::data

namespace game::scene {
/**
 * @brief 标题场景类，提供4个按钮：开始游戏、加载游戏、帮助、退出
 */
class TitleScene final : public engine::scene::Scene {
public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param scene_manager 场景管理器
     * @param game_play_state 指向游戏玩法状态的共享指针
     */
    TitleScene(engine::core::Context& context
             , engine::scene::SceneManager& scene_manager
             , std::shared_ptr<game::data::SessionData> session_data = nullptr);
    ~TitleScene() override = default;

    // 禁止拷贝和移动
    TitleScene(const TitleScene&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    TitleScene(TitleScene&&) = delete;
    TitleScene& operator=(TitleScene&&) = delete;

    // --- 核心方法 --- //
    void update(sf::Time delta) override;

private:
    // 初始化 UI 元素
    void create_ui();

    // 按钮回调函数
    void on_start_game_click();
    void on_load_game_click();
    void on_helps_click();
    void on_quit_click();

    std::shared_ptr<game::data::SessionData> session_data_;
};
} // namespace game::scenes