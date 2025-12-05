#pragma once
#include <SFML/System/Time.hpp>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace game::component {
    class PlayerComponent;
} // namespace game::component

namespace game::component::state {
/**
 * @brief 玩家状态机的抽象基类。
 */
class PlayerState {
    friend class game::component::PlayerComponent;
public:
    explicit PlayerState(PlayerComponent* player_component);
    virtual ~PlayerState() = default;

    // 禁止拷贝和移动
    PlayerState(const PlayerState&) = delete;
    PlayerState& operator=(const PlayerState&) = delete;
    PlayerState(PlayerState&&) = delete;
    PlayerState& operator=(PlayerState&&) = delete;

    void play_animation(std::string_view animation_name);      ///< @brief 播放指定名称的动画，使用 AnimationComponent 的方法

protected:
    /**
     * @brief 过渡到下一个状态，设置初始状态后无需重复传入 PlayerComponent 指针
     * @param Next 下一个状态类名
     * @param Args 下一个状态的构造参数
     */
    template<typename Next, typename... Args>
    void transition(Args&&... args) {
        next_state_ = std::make_unique<Next>(player_component_obs_, std::forward<Args>(args)...);
    }

    // 核心状态方法
    virtual void handle_input(engine::core::Context& context) = 0;              ///< @brief 处理输入
    virtual void update(sf::Time delta, engine::core::Context& context) = 0;    ///< @brief 更新

    PlayerComponent* player_component_obs_ = nullptr;   ///< @brief 指向拥有此状态的玩家组件
    std::unique_ptr<PlayerState> next_state_ = nullptr; ///< @brief 指向下一个节点的指针
};
} // namespace game::component::state