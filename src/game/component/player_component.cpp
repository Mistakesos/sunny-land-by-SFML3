#include "player_component.hpp"
#include "idle_state.hpp"
#include "input_manager.hpp"
#include "transform_component.hpp"
#include "physics_component.hpp"
#include "sprite_component.hpp"
#include "game_object.hpp"
#include <spdlog/spdlog.h>

namespace game::component {
PlayerComponent::PlayerComponent(engine::object::GameObject* owner)
    : Component{owner} {
    // 获取必要的组件
    transform_component_obs_ = owner_->get_component<engine::component::TransformComponent>();
    physics_component_obs_ = owner_->get_component<engine::component::PhysicsComponent>();
    sprite_component_obs_ = owner_->get_component<engine::component::SpriteComponent>();

    // 设为以脚底中心为原点
    sf::Sprite& sprite = sprite_component_obs_->get_sprite();
    auto size = sprite.getLocalBounds().size;
    transform_component_obs_->set_origin({size.x / 2.f, size.y});
    
    // 检查必要组件是否存在
    if (!transform_component_obs_ || !physics_component_obs_ || !sprite_component_obs_) {
        spdlog::error("Player 对象缺少必要组件！");
    }

    // 初始化状态机
    current_state_ = std::make_unique<state::IdleState>(this);
    if (!current_state_) {
        spdlog::error("初始化玩家状态失败（make_unique 返回空指针）！");
    }
    spdlog::debug("PlayerComponent 初始化完成。");
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