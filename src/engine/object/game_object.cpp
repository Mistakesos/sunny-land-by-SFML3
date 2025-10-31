#include "game_object.hpp"
#include <SFML/System/Time.hpp>
#include <ranges>

engine::object::GameObject::GameObject(std::string_view name, std::string_view tag)
    : name_{name}
    , tag_{tag} {
}

engine::object::GameObject::~GameObject() = default;

void engine::object::GameObject::set_name(std::string_view name) {
    this->name_ = name;
}

std::string_view engine::object::GameObject::get_name() const {
    return this->name_;
}

void engine::object::GameObject::set_tag(std::string_view tag) {
    this->tag_ = tag;
}

std::string_view engine::object::GameObject::get_tag() const {
    return this->tag_;
}

void engine::object::GameObject::set_need_remove(bool need_remove) {
    this->need_remove_ = need_remove;
}

bool engine::object::GameObject::is_need_remove() const {
    return this->need_remove_;
}

void engine::object::GameObject::handle_input() {
    for (auto& component : components_ | std::views::values) {
        component->handle_input();
    }
}

void engine::object::GameObject::update(sf::Time delta) {
    for (auto& component : components_ | std::views::values) {
        component->update(delta);
    }
}

void engine::object::GameObject::render() {
    for (auto& component : components_ | std::views::values) {
        component->render();
    }
}