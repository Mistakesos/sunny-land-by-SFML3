#pragma once
#include "component.hpp"
#include "game_object.hpp"
#include "player_state.hpp"
#include <memory>
#include <typeindex>
#include <utility>

namespace engine::input {
    class InputManager;
} // namespace engine::input

namespace engine::component {
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
} // namespace engine::component

namespace game::component::state {
    class PlayerState;
} // namespace game::component::state

namespace game::component {
/**
 * @brief 处理玩家输入、状态和控制 GameObject 移动的组件
 *        使用状态模式管理 Idle、Walk、Jump、Fall 等状态
 */
class PlayerComponent final : public engine::component::Component {
    friend class engine::object::GameObject;
public:
    PlayerComponent(engine::object::GameObject* owner);
    ~PlayerComponent() override = default;

    // 禁止拷贝和移动
    PlayerComponent(const PlayerComponent&) = delete;
    PlayerComponent& operator=(const PlayerComponent&) = delete;
    PlayerComponent(PlayerComponent&&) = delete;
    PlayerComponent& operator=(PlayerComponent&&) = delete;

    // setter-getter
    engine::component::TransformComponent* get_transform_component() const { return transform_component_obs_; }
    engine::component::SpriteComponent* get_sprite_component() const { return sprite_component_obs_; }
    engine::component::PhysicsComponent* get_physics_component() const { return physics_component_obs_; }

    void set_is_dead(bool is_dead) { is_dead_ = is_dead; }                ///< @brief 设置玩家是否死亡
    bool is_dead() const { return is_dead_; }                             ///< @brief 获取玩家是否死亡    
    void set_move_force(float move_force) { move_force_ = move_force; }   ///< @brief 设置水平移动力
    float get_move_force() const { return move_force_; }                  ///< @brief 获取水平移动力  
    void set_max_speed(float max_speed) { max_speed_ = max_speed; }       ///< @brief 设置最大移动速度
    float get_max_speed() const { return max_speed_; }                    ///< @brief 获取最大移动速度
    void set_friction_factor(float friction_factor) { friction_factor_ = friction_factor; }   ///< @brief 设置摩擦系数
    float get_friction_factor() const { return friction_factor_; }        ///< @brief 获取摩擦系数
    void set_jump_velocity(float jump_vel) { jump_vel_ = jump_vel; }      ///< @brief 设置跳跃速度
    float get_jump_velocity() const { return jump_vel_; }                 ///< @brief 获取跳跃速度

private:
    // 核心循环函数
    void handle_input(engine::core::Context& context) override;
    void update(sf::Time delta, engine::core::Context& context) override;
    
    engine::component::TransformComponent* transform_component_obs_ = nullptr; // 指向 TransformComponent 的非拥有指针
    engine::component::SpriteComponent* sprite_component_obs_ = nullptr;
    engine::component::PhysicsComponent* physics_component_obs_ = nullptr;

    std::unique_ptr<state::PlayerState> current_state_;
    bool is_dead_ = false;

    // --- 移动相关参数 ---
    float move_force_ = 200.f;         ///< @brief 水平移动力
    float max_speed_ = 120.f;          ///< @brief 最大移动速度 (像素/秒)
    float friction_factor_ = 0.85f;    ///< @brief 摩擦系数 (Idle时缓冲效果，每帧乘以此系数)
    float jump_vel_ = 350.f;           ///< @brief 跳跃速度 (按下"jump"键给的瞬间向上的速度)
};
} // namespace game::component