#include "camera.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace engine::render {
Camera::Camera(sf::RenderWindow* window, std::optional<sf::FloatRect> limit_bounds)
    : window_obs_{window}
    , world_view_{window->getDefaultView()}
    , ui_view_{window->getDefaultView()}
    , limit_bounds_{limit_bounds} {
    spdlog::trace("Camera 初始化成功");
    world_view_.zoom(0.5f);
    ui_view_.zoom(0.5f);
    world_view_.setCenter(world_view_.getSize() / 2.f);
    ui_view_.setCenter(ui_view_.getSize() / 2.f);
    
    window_obs_->setView(world_view_);
}

void Camera::update(const sf::Time& delta_time)
{
    // if (target_ == nullptr) return;
    
    // // 获取目标位置
    // sf::Vector2f target_pos = target_->get_position();
    
    // // 计算目标位置 (让目标位于视口中心)
    // // 注意：position_是相机左上角，所以要让目标在中心需要减去视口的一半
    // sf::Vector2f desired_position = target_pos - size_ / 2.0f;

    // // 计算当前位置与目标位置的距离
    // float dx = desired_position.x - position_.x;
    // float dy = desired_position.y - position_.y;
    // float distance = std::sqrt(dx * dx + dy * dy);
    
    // constexpr float snap_threshold = 1.0f;

    // if (distance < snap_threshold) {
    //     // 如果距离小于阈值，直接吸附到目标位置
    //     position_ = desired_position;
    // } else {
    //     // 使用线性插值平滑移动
    //     // 等效于 glm::mix: current + (desired - current) * factor
    //     float t = smooth_speed_ * delta_time;
    //     position_.x = position_.x + (desired_position.x - position_.x) * t;
    //     position_.y = position_.y + (desired_position.y - position_.y) * t;
        
    //     // 四舍五入到整数
    //     position_.x = std::round(position_.x);
    //     position_.y = std::round(position_.y);
    // }

    // clamp_position();
}

void Camera::set_world_view_center(sf::Vector2f center) {
    world_view_.setCenter(center);
    clamp_position();
}

void Camera::set_ui_view_center(sf::Vector2f center) {
    ui_view_.setCenter(center);
    clamp_position();
}

void Camera::move(const sf::Vector2f& offset) {
    world_view_.move(offset);
    clamp_position();
    window_obs_->setView(world_view_);
}

void Camera::set_limit_bounds(std::optional<sf::FloatRect> limit_bounds) {
    limit_bounds_ = limit_bounds;
    clamp_position();
}

void Camera::set_target(engine::component::TransformComponent* target) {
    target_obs_ = target;
}

engine::component::TransformComponent* Camera::get_target() const {
    return target_obs_;
}

const sf::Vector2f Camera::get_world_view_center() const {
    return world_view_.getCenter();
}

const sf::Vector2f Camera::get_ui_view_center() const {
    return ui_view_.getCenter();
}

void Camera::clamp_position() {
    if (!limit_bounds_.has_value()) return;
    
    const sf::FloatRect& bounds = limit_bounds_.value();
    
    // 边界检查需要确保相机视图（position 到 position + viewport_size）在 limit_bounds 内
    if (bounds.size.x > 0 && bounds.size.y > 0) {
        // 计算允许的相机位置范围
        sf::Vector2f min_cam_pos = bounds.position;
        sf::Vector2f max_cam_pos = bounds.position + bounds.size - get_world_view_size();

        // 确保 max_cam_pos 不小于 min_cam_pos (视口可能比世界还大)
        max_cam_pos.x = std::max(min_cam_pos.x, max_cam_pos.x);
        max_cam_pos.y = std::max(min_cam_pos.y, max_cam_pos.y);

        // 手动实现clamp
        auto position = world_view_.getCenter();
        position.x = std::clamp(position.x, min_cam_pos.x, max_cam_pos.x);
        position.y = std::clamp(position.y, min_cam_pos.y, max_cam_pos.y);
    }
}

sf::Vector2f Camera::world_to_screen(const sf::Vector2f& world_pos) const {
    // 将世界坐标减去相机左上角位置
    return window_obs_->mapPixelToCoords(static_cast<sf::Vector2i>(world_pos));
}

sf::Vector2f Camera::world_to_screen_with_parallax(const sf::Vector2f& world_pos, const sf::Vector2f& scroll_factor) const {
    // 1. 计算视差偏移：相机位置乘以滚动因子
    auto position = get_world_view_center();
    sf::Vector2f parallax_offset(position.x * scroll_factor.x, position.y * scroll_factor.y);
    
    // 2. 应用视差效果：世界坐标减去视差偏移
    sf::Vector2f parallax_adjusted_pos = world_pos - parallax_offset;
    
    // 3. 转换为屏幕坐标（与第一个函数保持一致）
    return window_obs_->mapPixelToCoords(static_cast<sf::Vector2i>(parallax_adjusted_pos));
}

sf::Vector2f Camera::screen_to_world(const sf::Vector2f& screen_pos) const {
    // 将屏幕坐标加上相机左上角位置
    return static_cast<sf::Vector2f>(window_obs_->mapCoordsToPixel(screen_pos));
}

sf::Vector2f Camera::get_world_view_size() const {
    return world_view_.getViewport().size;
}

sf::Vector2f Camera::get_ui_view_size() const {
    return ui_view_.getViewport().size;
}

std::optional<sf::FloatRect> Camera::get_limit_bounds() const {
    return limit_bounds_;
}

const sf::View& Camera::get_world_view() const {
    return world_view_;
}

const sf::View& Camera::get_ui_view() const {
    return ui_view_;
}
} // namespace engine::render