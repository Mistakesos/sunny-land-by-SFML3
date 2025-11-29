#pragma once
#include "component.hpp"
#include "ai_behavior.hpp"
#include <memory>

namespace game::component::ai {
    class AIBehavior;
} // namespace game::component::ai

namespace engine::component {
    class TransformComponent;
    class PhysicsComponent;
    class ColliderComponent;
    class AnimationComponent;
    class SpriteComponent;
} // namespace engine::component

namespace game::component {
/**
 * @brief 负责管理 GameObject 的 AI 行为。
 *
 * 使用策略模式，持有一个具体的 AIBehavior 实例来执行实际的 AI 逻辑。
 * 提供对 GameObject 其他关键组件的访问。
 */
class AIComponent final : public engine::component::Component {
    friend class engine::object::GameObject;
public:
    AIComponent(engine::object::GameObject* owner);
    ~AIComponent() override = default;

    // 禁止拷贝和移动
    AIComponent(const AIComponent&) = delete;
    AIComponent& operator=(const AIComponent&) = delete;
    AIComponent(AIComponent&&) = delete;
    AIComponent& operator=(AIComponent&&) = delete;

    void set_behavior(std::unique_ptr<ai::AIBehavior> behavior); ///< @brief 设置当前 AI 行为策略
    bool take_damage(int damage);        ///< @brief 处理伤害逻辑，返回是否造成伤害
    bool is_alive() const;               ///< @brief 检查对象是否存活

    // --- Setters and Getters ---
    engine::component::TransformComponent* get_transform_component() const { return transform_component_obs_; }
    engine::component::PhysicsComponent* get_physics_component() const { return physics_component_obs_; }
    engine::component::ColliderComponent* get_collider_component() const { return collider_component_obs_; }
    engine::component::SpriteComponent* get_sprite_component() const { return sprite_component_obs_; }
    engine::component::AnimationComponent* get_animation_component() const { return animation_component_obs_; }

private:
    // 核心循环方法
    void update(sf::Time delta, engine::core::Context&) override;

    std::unique_ptr<ai::AIBehavior> current_behavior_ = nullptr; ///< @brief 当前 AI 行为策略
    /* 未来可添加一些敌人属性 */

    // --- 缓存组件指针 ---
    engine::component::TransformComponent* transform_component_obs_ = nullptr;
    engine::component::PhysicsComponent* physics_component_obs_ = nullptr;
    engine::component::ColliderComponent* collider_component_obs_ = nullptr;
    engine::component::SpriteComponent* sprite_component_obs_ = nullptr;
    engine::component::AnimationComponent* animation_component_obs_ = nullptr;
};
} // namespace game::component
