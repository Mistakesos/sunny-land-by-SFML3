#include "player_component.hpp"
#include "idle_state.hpp"
#include "hurt_state.hpp"
#include "dead_state.hpp"
#include "input_manager.hpp"
#include "transform_component.hpp"
#include "physics_component.hpp"
#include "sprite_component.hpp"
#include "animation_component.hpp"
#include "health_component.hpp"
#include "game_object.hpp"
#include <spdlog/spdlog.h>

namespace game::component {
PlayerComponent::PlayerComponent(engine::object::GameObject* owner)
    : Component{owner} {
    // 获取必要的组件
    transform_component_obs_ = owner_->get_component<engine::component::TransformComponent>();
    physics_component_obs_ = owner_->get_component<engine::component::PhysicsComponent>();
    sprite_component_obs_ = owner_->get_component<engine::component::SpriteComponent>();
    animation_component_obs_ = owner_->get_component<engine::component::AnimationComponent>();
    health_component_obs_ = owner->get_component<engine::component::HealthComponent>();

    // 设为以脚底中心为原点
    sf::Sprite& sprite = sprite_component_obs_->get_sprite();
    auto size = sprite.getLocalBounds().size;
    transform_component_obs_->set_origin({size.x / 2.f, size.y});
    
    // 检查必要组件是否存在
    if (!transform_component_obs_ || !physics_component_obs_ || !sprite_component_obs_) {
        spdlog::error("Player 对象缺少必要组件！");
    }

    // <-!!!-> // 初始化状态机，传入一次 this，后续使用 transition 可以省略第一个参数
    current_state_ = std::make_unique<state::IdleState>(this);
    if (!current_state_) {
        spdlog::error("初始化玩家状态失败（make_unique 返回空指针）！");
    }
    spdlog::debug("PlayerComponent 初始化完成。");
}

bool PlayerComponent::take_damage(int damage) {
    if (is_dead_ || !health_component_obs_ || damage <= 0) {
        spdlog::warn("玩家已死亡或却少必要组件，并未造成伤害。");
        return false;
    }

    bool success = health_component_obs_->take_damage(damage);
    if (!success) return false;
    // --- 成功造成伤害了，根据是否存活决定状态切换
    if (health_component_obs_->is_alive()) {
        spdlog::debug("玩家受到了 {} 点伤害，当前生命值: {}/{}。",
                     damage, health_component_obs_->get_current_health(), health_component_obs_->get_max_health());
        // 切换到受伤状态
        current_state_->transition<state::HurtState>();
    } else {
        spdlog::debug("玩家死亡。");
        is_dead_ = true;
        // 切换到死亡状态
        current_state_->transition<state::DeadState>();
    }
    return true;
}

void PlayerComponent::handle_input(engine::core::Context& context) {
    if (current_state_) current_state_->handle_input(context);

    // 只检查一次
    if (current_state_->next_state_) {
        current_state_ = std::move(current_state_->next_state_);
    }
}

void PlayerComponent::update(sf::Time delta, engine::core::Context& context) {
    if (current_state_) current_state_->update(delta, context);

    if (current_state_->next_state_) {
        current_state_ = std::move(current_state_->next_state_);
    }
}
} // namespace game::component