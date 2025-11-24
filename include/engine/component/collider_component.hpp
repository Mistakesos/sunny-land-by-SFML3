#pragma once
#include "component.hpp"
#include "collider.hpp"
#include <memory>

namespace engine::component {
    class TransformComponent;
} // namespace engine::component

namespace engine::component {
/**
 * @brief 碰撞器组件
 * 
 * 持有 Collider 对象，并提供获取世界坐标下碰撞形状的方法
 */
class ColliderComponent final : public Component {
    friend class engine::object::GameObject;
public:
    /**
     * @brief 构造函数
     * @param collider 指向 Collider 实例的 unique_ptr 所有权将被转移
     * @param is_trigger 此碰撞器是否为触发器
     * @param is_active 此碰撞器是否激活
     */
    explicit ColliderComponent(
        engine::object::GameObject* owner,
        std::unique_ptr<engine::physics::Collider> collider,
        bool is_trigger = false,
        bool is_active = true
    );
    ~ColliderComponent() = default;

    // --- Getters ---
    TransformComponent* get_transform() const { return transform_obs_; }             ///< @brief 获取缓存的TransformComponent
    const engine::physics::Collider* get_collider() const { return collider_.get(); }///< @brief 获取 Collider 对象
    const sf::Vector2f& get_offset() const { return offset_; }                       ///< @brief 获取当前计算出的偏移量
    sf::FloatRect get_world_aabb() const;                                            ///< @brief 获取世界坐标系下的最小轴对齐包围盒（AABB）
    bool is_trigger() const { return is_trigger_; }                                  ///< @brief 检查此碰撞器是否为触发器
    bool is_active() const { return is_active_; }                                    ///< @brief 检查此碰撞器是否激活

    void set_offset(sf::Vector2f offset) { offset_ = std::move(offset); }            ///< @brief 设置偏移量
    void set_trigger(bool is_trigger) { is_trigger_ = is_trigger; }                  ///< @brief 设置此碰撞器是否为触发器
    void set_active(bool is_active) { is_active_ = is_active; }                      ///< @brief 设置此碰撞器是否激活

private:
    // 核心循环方法
    void update(sf::Time, engine::core::Context&) override {}

    TransformComponent* transform_obs_ = nullptr;           ///< @brief TransformComponent 的非拥有指针

    std::unique_ptr<engine::physics::Collider> collider_;   ///< @brief 拥有的碰撞器对象
    sf::Vector2f offset_ = {0.f, 0.f};                      ///< @brief 碰撞器（最小包围盒的）左上角相对于变换原点的偏移量

    bool is_trigger_ = false;                               ///< @brief 是否为触发器（仅检测碰撞，不产生物理响应）
    bool is_active_ = true;                                 ///< @brief 是否激活
};
}