#pragma once
#include "ai_behavior.hpp"
#include <SFML/System/Time.hpp>

namespace game::component::ai {
/**
 * @brief AI 行为：在指定范围内左右巡逻。
 *
 * 遇到墙壁或到达巡逻边界时会转身。
 */
class UpDownBehavior final : public AIBehavior {
    friend class game::component::AIComponent;
public:
    /**
     * @brief 构造函数。
     * @param min_y 巡逻范围的最小 y 坐标。
     * @param max_y 巡逻范围的最大 y 坐标。
     * @param speed 移动速度。
     */
    UpDownBehavior(AIComponent* ai_component, float min_y, float max_y, float speed = 50.f);
    ~UpDownBehavior() override = default;

    // 禁止拷贝和移动
    UpDownBehavior(const UpDownBehavior&) = delete;
    UpDownBehavior& operator=(const UpDownBehavior&) = delete;
    UpDownBehavior(UpDownBehavior&&) = delete;
    UpDownBehavior& operator=(UpDownBehavior&&) = delete;

private:
    void update(sf::Time) override;

    float patrol_min_y_ = 0.f;      ///< @brief 巡逻范围的上边界
    float patrol_max_y_ = 0.f;      ///< @brief 巡逻范围的下边界
    float move_speed_ = 50.f;       ///< @brief 移动速度 (像素/秒)
    bool moving_down_ = false;      ///< @brief 当前是否向下移动
};
} // namespace game::component::ai