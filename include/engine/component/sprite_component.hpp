#pragma once
#include "component.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <string_view>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::component {
class TransformComponent;

/**
 * @brief 管理 GameObject 的视觉表示，通过持有一个 Sprite 对象。
 *
 * 协调 Sprite 数据和渲染逻辑，并与 TransformComponent 交互。
 */
class SpriteComponent final : public engine::component::Component {
    friend class engine::object::GameObject;            // 友元不能继承，必须每个子类单独添加
public:
    SpriteComponent(const sf::Texture& texture);
    SpriteComponent(sf::Sprite&& sprite);
    ~SpriteComponent() override = default;

    sf::Sprite& get_sprite();                           ///< @brief 获取精灵
    bool get_hidden();
    void set_hidden(bool hide);

private:
    void update(sf::Time delta, engine::core::Context& context) override {} ///< @brief 更新函数留空
    void render(engine::core::Context& context) override;                   ///< @brief 渲染函数需要覆盖

    TransformComponent* transform_obs_ = nullptr;                           ///< @brief 变换组件的观察指针

    sf::Sprite sprite_;                                                     ///< @brief 内部储存的精灵
    bool is_hidden_ = false;                                                ///< @brief 是否隐藏（不渲染）
};
} // namespace engine::component