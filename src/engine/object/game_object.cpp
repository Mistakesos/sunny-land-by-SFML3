#include "game_object.hpp"
#include <SFML/System/Time.hpp>
#include <ranges>

namespace engine::object {
GameObject::GameObject(std::string_view name, std::string_view tag)
    : name_{name}
    , tag_{tag} {
}

void GameObject::set_name(std::string_view name) {
    this->name_ = name;
}

std::string_view GameObject::get_name() const {
    return this->name_;
}

void GameObject::set_tag(std::string_view tag) {
    this->tag_ = tag;
}

std::string_view GameObject::get_tag() const {
    return this->tag_;
}

void GameObject::set_need_remove(bool need_remove) {
    this->need_remove_ = need_remove;
}

bool GameObject::is_need_remove() const {
    return this->need_remove_;
}

void GameObject::handle_input(engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->handle_input(context);
    }
}

void GameObject::update(sf::Time delta, engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->update(delta, context);
    }
}

void GameObject::render(engine::core::Context& context) {
    for (auto& component : components_ | std::views::values) {
        component->render(context);
    }
}
} // namespace engine::object