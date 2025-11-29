#pragma once
#include "ai_behavior.hpp"
#include <SFML/System/Time.hpp>

namespace game::component { 
    class AIComponent; 
} // namespace game::component

namespace game::component::ai {
/**
 * @brief AI 行为策略的抽象基类。
 */
class AIBehavior {
    friend class game::component::AIComponent;
public:
    /**
     * 构造即进入，没有设计 enter/exit
     */
    AIBehavior(AIComponent* ai_component);
    virtual ~AIBehavior() = default;

    //禁止移动和拷贝
    AIBehavior(const AIBehavior&) = delete;
    AIBehavior& operator=(const AIBehavior&) = delete;
    AIBehavior(AIBehavior&&) = delete;
    AIBehavior& operator=(AIBehavior&&) = delete;

protected:
    /**
     * @brief 设置下一个行为，设置初始状态后无需重复传入 AIComponent 引用
     * @param Next 下一个行为类名
     * @param Args 下一个行为的构造参数
     */
    template<typename Next, typename... Args>
    void transition(Args&&... args) {
        next_behavior_ = std::make_unique<Next>(ai_component_obs_, std::forward<Args>(args)...);
    }

    virtual void update(sf::Time) = 0;                       ///< @brief 更新 AI 行为逻辑(具体策略)，必须实现

    AIComponent* ai_component_obs_;                          ///< @brief 内部缓存的 AIComponent指针
    std::unique_ptr<AIBehavior> next_behavior_ = nullptr;    ///< @brief 指向下一个节点的指针
};
} // namespace game::component::ai
