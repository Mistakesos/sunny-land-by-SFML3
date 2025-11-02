#include "animation.hpp"

namespace engine::render {
Animation::Animation(std::string_view name, bool loop) {
}

void Animation::add_frame(const sf::FloatRect& source_rect, const sf::Time& duration) {
}

// const AnimationFrame& Animation::get_frame(const sf::Time& time) const {
//     return AnimationFrame();
// }

const std::vector<AnimationFrame>& Animation::get_frames_vec() const {
    return frames_;
}

std::uint32_t Animation::get_frame_count() const {
    return frames_.size();
}

std::string_view Animation::get_name() const {
    return name_;
}

void Animation::set_name(std::string_view name) {
    this->name_ = name;
}

const sf::Time& Animation::get_total_durationn() const {
    return this->total_duration_;
}

void Animation::set_looping(bool loop) {
    this->is_loop_ = loop;
}

bool Animation::is_looping() const {
    return this->is_loop_;
}

bool Animation::is_empty() const {
    return frames_.empty();
}
} // namespace engine::render