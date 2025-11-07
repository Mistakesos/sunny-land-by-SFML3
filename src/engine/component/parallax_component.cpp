#include "parallax_component.hpp"
#include "transform_component.hpp"
#include "game_object.hpp"
#include "render.hpp"
#include "context.hpp"

namespace engine::component {
ParallaxComponent::ParallaxComponent(const sf::Texture& texture, const sf::Vector2f& scroll_factor, sf::Vector2<bool> repeat)
    : sprite_{texture}
    , scroll_factor_{std::move(scroll_factor)}
    , repeat_{std::move(repeat)} {
    ///< @attention transform_obs_ 在渲染函数调用时初始化，并确保了只初始化一次
}

ParallaxComponent::~ParallaxComponent() {
}

void ParallaxComponent::set_sprite(sf::Sprite& sprite) {
    sprite_ = sprite;
}

void ParallaxComponent::set_scroll_factor(sf::Vector2f factor) {
    scroll_factor_ = std::move(factor);
}

void ParallaxComponent::set_repeat(sf::Vector2<bool> repeat) {
    repeat_ = std::move(repeat);
}

void ParallaxComponent::set_hidden(bool hidden) {
    is_hidden_ = hidden;
}

const sf::Sprite& ParallaxComponent::get_sprite() const {
    return sprite_;
}

const sf::Vector2f& ParallaxComponent::get_scroll_factor() const {
    return scroll_factor_;
}

const sf::Vector2<bool>& ParallaxComponent::get_repeat() const {
    return repeat_;
}

bool ParallaxComponent::is_hidden() const {
    return is_hidden_;
}

void ParallaxComponent::render(engine::core::Context& context) {
    if (owner_ && !transform_obs_) {
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
    
    // 直接调用视差滚动绘制函数
    context.get_renderer().draw_parallax(context.get_camera(), sprite_, scroll_factor_, repeat_, transform_obs_->get_scale());  
}
} // namespace engine::component