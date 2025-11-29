#pragma once
#include "component.hpp"
#include <SFML/System/Vector2.hpp>

namespace engine::physics {
    class PhysicsEngine;
} // namespace engine::physics

namespace engine::component {
class TransformComponent;

/**
 * @brief 管理 GameObject 的物理属性
 *
 * 储存速度、质量、力和中立设置、与 PhysicsEngine 交互
 */
class PhysicsComponent final : public Component {
    friend class engine::object::GameObject;
public:
    /**
     * @brief 构造函数  
     * 
     * @param physics_engine 指向PhysicsEngine的指针，不能为nullptr
     * @param use_gravity 物体是否受重力影响，默认true
     * @param mass 物体质量，默认1.0
     */
    PhysicsComponent(engine::object::GameObject* owner, engine::physics::PhysicsEngine* physics_engine, bool use_gravity = true, float mass = 1.f);
    ~PhysicsComponent() override;

    // 删除复制/移动操作
    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;
    PhysicsComponent(PhysicsComponent&&) = delete;
    PhysicsComponent& operator=(PhysicsComponent&&) = delete;

    // PhysicsEngine使用的物理方法
    void add_force(const sf::Vector2f& force) { if (enabled_) force_ += force; } ///< @brief 添加力
    void clear_force() { force_ = {0.f, 0.f}; }                                  ///< @brief 清除力
    const sf::Vector2f& get_force() const { return force_; }                     ///< @brief 获取当前力
    float get_mass() const { return mass_; }                                     ///< @brief 获取质量
    bool is_enabled() const { return enabled_; }                                 ///< @brief 获取组件是否启用
    bool is_use_gravity() const { return use_gravity_; }                         ///< @brief 获取组件是否受重力影响

    // 设置器/获取器
    void set_enabled(bool enabled) { enabled_ = enabled; }                           ///< @brief 设置组件是否启用
    void set_mass(float mass) { mass_ = (mass >= 0.f) ? mass : 1.f; }                ///< @brief 设置质量，质量不能为负
    void set_use_gravity(bool use_gravity) { use_gravity_ = use_gravity; }           ///< @brief 设置组件是否受重力影响
    void set_velocity(sf::Vector2f velocity) { velocity_ = std::move(velocity); }    ///< @brief 设置速度
    const sf::Vector2f& get_velocity() const { return velocity_; }                   ///< @brief 获取当前速度
    TransformComponent* get_transform() const { return transform_obs_; }             ///< @brief 获取TransformComponent指针
    
    // --- 碰撞状态访问与修改 (供 PhysicsEngine 使用) ---
    /** @brief 重置所有碰撞标志 (在物理更新开始时调用) */
    void reset_collision_flags() {
        collided_below_ = false;
        collided_above_ = false;
        collided_left_ = false;
        collided_right_ = false;
        collided_ladder_ = false;
        is_on_top_ladder_ = false;
    }

    void set_collided_below(bool collided) { collided_below_ = collided; }    ///< @brief 设置下方碰撞标志
    void set_collided_above(bool collided) { collided_above_ = collided; }    ///< @brief 设置上方碰撞标志
    void set_collided_left(bool collided) { collided_left_ = collided; }      ///< @brief 设置左方碰撞标志
    void set_collided_right(bool collided) { collided_right_ = collided; }    ///< @brief 设置右方碰撞标志
    void set_collided_ladder(bool collided) { collided_ladder_ = collided; }  ///< @brief 设置梯子碰撞标志
    void set_on_top_ladder(bool on_top) { is_on_top_ladder_ = on_top; }       ///< @brief 设置是否在梯子顶层
    
    bool has_collided_below() const { return collided_below_; }       ///< @brief 检查是否与下方发生碰撞
    bool has_collided_above() const { return collided_above_; }       ///< @brief 检查是否与上方发生碰撞
    bool has_collided_left() const { return collided_left_; }         ///< @brief 检查是否与左方发生碰撞
    bool has_collided_right() const { return collided_right_; }       ///< @brief 检查是否与右方发生碰撞    
    bool has_collided_ladder() const { return collided_ladder_; }     ///< @brief 检查是否与梯子发生碰撞
    bool is_on_top_ladder() const { return is_on_top_ladder_; }       ///< @brief 检查是否在梯子顶层

private:
    // 核心循环方法
    void update(sf::Time, engine::core::Context&) override {}

public:
    sf::Vector2f velocity_ = {0.f, 0.f};    ///< @brief 物体的速度,公共成员，方便 PhysisEngine访问更新

private:
    engine::physics::PhysicsEngine* physics_engine_obs_ = nullptr;      ///< @brief 指向物理引擎的观察指针
    TransformComponent* transform_obs_ = nullptr;                       ///< @brief 变换组件的观察指针

    sf::Vector2f force_ = {0.f, 0.f};           ///< @brief 当前帧受到的力
    float mass_ = 1.f;                          ///< @brief 物体质量（默认1）
    bool use_gravity_ = true;                   ///< @brief 物体是否受重力影响
    bool enabled_ = true;                       ///< @brief 组件是否激活

    // --- 碰撞状态标志 ---
    bool collided_below_ = false;
    bool collided_above_ = false;
    bool collided_left_ = false;
    bool collided_right_ = false;
    bool collided_ladder_ = false;      ///< @brief 是否与梯子发生碰撞
    bool is_on_top_ladder_ = false;     ///< @brief 是否在梯子顶层（梯子上方没有瓦片）
};
} // namespace engine::component