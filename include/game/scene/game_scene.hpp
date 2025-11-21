#pragma once
#include "scene.hpp"
#include <SFML/System/Time.hpp>
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

    engine::object::GameObject* player_obs_ = nullptr; ///< @brief 保存测试对象的指针，方便访问
};
}