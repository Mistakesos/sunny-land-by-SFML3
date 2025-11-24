#pragma once
#include "scene.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace engine::object {
    class GameObject;
} // namespace engine::object

namespace game::scene {
/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等
 */
class GameScene final : public engine::scene::Scene {
public:
    GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);
    ~GameScene();
    // 覆盖场景类的核心方法
    void update(sf::Time delta) override;
    void render() override;
    void handle_input() override;
    
private:
    [[nodiscard]] bool init_level();
    [[nodiscard]] bool init_player();
    [[nodiscard]] bool init_enemy_and_item();

    void handle_object_collisions();        ///< @brief 处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息）
    void handle_tile_triggers();            ///< @brief 处理瓦片触发事件（从PhysicsEngine获取信息）
    void player_vs_enemy_collision(engine::object::GameObject* player, engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
    void player_vs_item_collision(engine::object::GameObject* player, engine::object::GameObject* item);    ///< @brief 玩家与道具碰撞处理

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param center_pos 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void create_effect(sf::Vector2f center_pos, std::string_view tag);

    engine::object::GameObject* player_obs_ = nullptr; ///< @brief 保存测试对象的指针，方便访问
};
}