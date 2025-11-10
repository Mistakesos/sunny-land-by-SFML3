#include "sprite_component.hpp"
#include "transform_component.hpp"
#include "game_object.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "context.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
SpriteComponent::SpriteComponent(engine::object::GameObject* owner, const sf::Texture& texture)
    : Component{owner}
    , sprite_{texture} {
    ///< @attention transform_obs_ 在渲染函数调用时初始化，并确保了只初始化一次
}

SpriteComponent::SpriteComponent(engine::object::GameObject* owner, sf::Sprite&& sprite) 
    : Component{owner}
    , sprite_{sprite} {
}

sf::Sprite& SpriteComponent::get_sprite() {
   return this->sprite_;
}

bool SpriteComponent::get_hidden() {
    return this->is_hidden_;
}

void SpriteComponent::set_hidden(bool hide) {
    this->is_hidden_ = hide;
}

void SpriteComponent::render(engine::core::Context& context) {
    if (!transform_obs_) {
        transform_obs_ = owner_->get_component<TransformComponent>();
        if (!transform_obs_) {
            spdlog::warn(
                "GameObject '{}' 上的 SpriteComponent 需要一个 TransformComponent，但未找到。",
                owner_->get_name()
            );
        }
    }
    if (is_hidden_) {
        return;
    }

    // 获取变换信息（考虑偏移量）
    const sf::Vector2f& origin = transform_obs_->get_origin();
    const sf::Vector2f& pos = transform_obs_->get_position();
    const sf::Vector2f& scale = transform_obs_->get_scale();
    sf::Angle rotation = transform_obs_->get_rotation();

    sprite_.setOrigin(origin);
    sprite_.setPosition(pos);
    sprite_.setScale(scale);
    sprite_.setRotation(rotation);
    
    // 执行绘制
    context.get_renderer().draw_sprite(context.get_camera(), sprite_);
}
} // namespace engine::core