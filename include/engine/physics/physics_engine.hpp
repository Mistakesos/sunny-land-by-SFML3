#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace engine::component {
    class PhysicsComponent;
} // namespace engine::component

namespace engine::physics {
class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    // 禁止拷贝和移动
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void register_component(engine::component::PhysicsComponent* component);     ///< @brief 注册物理组件
    void unregister_component(engine::component::PhysicsComponent* component);   ///< @brief 注销物理组件

    void update(sf::Time delta);

    // 设置器/获取器
    void set_gravity(sf::Vector2f gravity) { gravity_ = std::move(gravity); }           ///< @brief 设置全局重力加速度
    const sf::Vector2f& get_gravity() const { return gravity_; }                        ///< @brief 获取当前的全局重力加速度
    void set_max_speed(sf::Vector2f max_speed) { max_speed_ = std::move(max_speed); }   ///< @brief 设置最大速度
    sf::Vector2f get_max_speed() const { return max_speed_; }                           ///< @brief 获取当前的最大速度

private:
    std::vector<engine::component::PhysicsComponent*> components_;  ///< @brief 注册的物理组件容器，非拥有指针
    sf::Vector2f gravity_ = {0.f, 980.f};       ///< @brief 默认重力值（像素/秒^2,相当于100像素对应现实1米）
    sf::Vector2f max_speed_ = {500.f, 500.f};   ///< @brief 最大速度（像素/秒）
};
} // namespace engine::physics