#pragma once
#include "scene.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace engine::object {
    class GameObject;
} // namespace engine::object

namespace game::data {
    class SessionData;
} // namespace game::data

namespace engine::ui {
    class UILabel;
    class UIPanel;
    class UIImage;
} // namespace engine::ui

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context
            , engine::scene::SceneManager& scene_manager
            , std::shared_ptr<game::data::SessionData> data = nullptr);
    ~GameScene();
    // 覆盖场景类的核心方法
    void update(sf::Time delta) override;
    void render() override;
    void handle_input() override;
    
private:
    [[nodiscard]] bool init_level();
    [[nodiscard]] bool init_player();
    [[nodiscard]] bool init_enemy_and_item();
    [[nodiscard]] bool init_ui();

    void handle_object_collisions();        ///< @brief 处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息）
    void handle_tile_triggers();            ///< @brief 处理瓦片触发事件（从PhysicsEngine获取信息）
    void handle_player_damage(int damage);  ///< @brief 处理玩家受伤（更新得分、UI等）
    void player_vs_enemy_collision(engine::object::GameObject* player, engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
    void player_vs_item_collision(engine::object::GameObject* player, engine::object::GameObject* item);    ///< @brief 玩家与道具碰撞处理

    void to_next_level(engine::object::GameObject* trigger);          ///< @brief 进入下一个关卡

    /// @brief 根据关卡名称获取对应的地图文件路径
    std::string level_name_to_path(std::string_view level_name) const { return "assets/maps/" + std::string(level_name) + ".tmj"; }

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param center_pos 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void create_effect(sf::Vector2f center_pos, std::string_view tag);

    // --- UI 相关函数 ---
    void create_score_ui();                           ///< @brief 创建得分UI
    void create_health_ui();                          ///< @brief 创建生命值UI (或最大生命值改变时重设)
    void add_score_with_ui(int score);                ///< @brief 增加得分，同时更新UI
    void heal_with_ui(int amount);                    ///< @brief 增加生命，同时更新UI
    void update_health_with_ui();                     ///< @brief 更新生命值UI (只适用最大生命值不变的情况)

    // --- 测试函数 ---
    void create_test_button();                       ///< @brief 创建测试按钮
    void test_button_clicked();                      ///< @brief 测试按钮点击事件

    std::shared_ptr<game::data::SessionData> game_session_data_ = nullptr;      ///< @brief 场景间共享数据，因此用shared_ptr
    engine::object::GameObject* player_obs_ = nullptr;                          ///< @brief 保存玩家对象的非拥有指针，方便访问

    engine::ui::UILabel* score_label_obs_ = nullptr;         ///< @brief 得分标签 (生命周期由UIManager管理，因此使用裸指针)
    engine::ui::UIPanel* health_panel_obs_ = nullptr;        ///< @brief 生命值图标面板
};
}