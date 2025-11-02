#include "component.hpp"

namespace engine::component {
void Component::set_owner(engine::object::GameObject* owner) {
    this->owner_ = owner;
}

engine::object::GameObject* Component::get_owner() const {
    return this->owner_;
}
} // namespace engine::component