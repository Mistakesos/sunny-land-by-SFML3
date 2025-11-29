#include "ai_behavior.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {
AIBehavior::AIBehavior(AIComponent* ai_component)
    : ai_component_obs_{ai_component} {
    if (!ai_component_obs_) {
        spdlog::error("出现错误！ai行为初始化时需要传入一个AIComponent指针！");
    }
}
} // namespace game::component::ai