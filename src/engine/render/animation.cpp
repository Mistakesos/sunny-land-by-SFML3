#include "animation.hpp"

engine::render::Animation::Animation(std::string_view name, bool loop) {
}

engine::render::Animation::~Animation() = default;

void engine::render::Animation::add_frame(const sf::FloatRect& source_rect, const sf::Time& duration) {
}

// const engine::render::AnimationFrame& engine::render::Animation::get_frame(const sf::Time& time) const {
//     return AnimationFrame();
// }

const std::vector<engine::render::AnimationFrame>& engine::render::Animation::get_frames_vec() const {
    return frames_;
}

std::uint32_t engine::render::Animation::get_frame_count() const {
    return frames_.size();
}

std::string_view engine::render::Animation::get_name() const {
    return name_;
}

void engine::render::Animation::set_name(std::string_view name) {
    this->name_ = name;
}

const sf::Time& engine::render::Animation::get_total_durationn() const {
    return this->total_duration_;
}

void engine::render::Animation::set_looping(bool loop) {
    this->is_loop_ = loop;
}

bool engine::render::Animation::is_looping() const {
    return this->is_loop_;
}

bool engine::render::Animation::is_empty() const {
    return frames_.empty();
}
