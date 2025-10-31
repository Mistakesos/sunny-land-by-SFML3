#include "component.hpp"
#include <SFML/System/Time.hpp>

engine::component::Component::Component() = default;

engine::component::Component::~Component() = default;

void engine::component::Component::set_owner(engine::object::GameObject* owner) {
    this->owner_ = owner;
}

engine::object::GameObject* engine::component::Component::get_owner() const {
    return this->owner_;
}
