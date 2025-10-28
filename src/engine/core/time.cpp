#include "time.hpp"

engine::core::Time::Time() = default;

engine::core::Time::~Time() = default;

const sf::Time& engine::core::Time::get_frame_duration() const {
    return this->time_per_frame_;
}

void engine::core::Time::set_time_scale(float scale) {
    this->time_scale_ = scale;
    this->time_per_frame_ *= scale;
}

float engine::core::Time::get_time_scale() {
    return this->time_scale_;
}

void engine::core::Time::set_target_fps(float fps) {
    this->TAEGET_FPS = fps;
    this->time_per_frame_ = sf::seconds(1.f / TAEGET_FPS);
}

float engine::core::Time::get_target_fps() const {
    return this->TAEGET_FPS;
}

void engine::core::Time::accumulate_frame_time() {
    elapsed_time += clock_.restart();
}

bool engine::core::Time::should_update() const {
    return elapsed_time > time_per_frame_;
}

void engine::core::Time::consume_update_time() {
    elapsed_time -= time_per_frame_;
}
