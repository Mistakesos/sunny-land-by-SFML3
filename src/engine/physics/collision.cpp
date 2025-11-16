#include "collision.hpp"
#include "collider_component.hpp"
#include "transform_component.hpp"
#include <algorithm>

namespace engine::physics::collision {
bool check_collision(const engine::component::ColliderComponent& a, const engine::component::ColliderComponent& b) {
    // 先排除不活跃或空指针情况
    if (!a.is_active() || !b.is_active()) return false;
    if (!a.get_collider() || !b.get_collider()) return false;

    // 使用 ColliderComponent 提供的世界坐标 AABB —— 统一来源，避免坐标/原点/缩放不一致的问题
    sf::FloatRect a_rect = a.get_world_aabb();
    sf::FloatRect b_rect = b.get_world_aabb();

    // 如果最小包围盒都不相交，可以直接返回 false（快速拒绝）
    if (!a_rect.findIntersection(b_rect)) {
        return false;
    }

    // 获取具体碰撞器类型（用于次级判定）
    const auto* a_collider = a.get_collider();
    const auto* b_collider = b.get_collider();
    const auto a_type = a_collider->get_type();
    const auto b_type = b_collider->get_type();

    // AABB vs AABB：上面已用矩形相交做过检测，直接返回 true
    if (a_type == engine::physics::ColliderType::Aabb && b_type == engine::physics::ColliderType::Aabb) {
        return true;
    }

    // 为后续圆心/半径及矩形位置计算提供方便的值
    sf::Vector2f a_pos{a_rect.position};
    sf::Vector2f a_size{a_rect.size};
    sf::Vector2f b_pos{b_rect.position};
    sf::Vector2f b_size{b_rect.size};

    // Circle vs Circle
    if (a_type == engine::physics::ColliderType::Circle && b_type == engine::physics::ColliderType::Circle) {
        // 两个圆的圆心 = 各自 AABB 的中心，半径 = AABB 宽度/2（按你的约定）
        sf::Vector2f a_center{a_pos.x + 0.5f * a_size.x, a_pos.y + 0.5f * a_size.y};
        sf::Vector2f b_center{b_pos.x + 0.5f * b_size.x, b_pos.y + 0.5f * b_size.y};
        float a_radius = 0.5f * a_size.x;
        float b_radius = 0.5f * b_size.x;
        return check_circle_overlap(a_center, a_radius, b_center, b_radius);
    }

    // AABB vs Circle (a AABB, b Circle)
    if (a_type == engine::physics::ColliderType::Aabb && b_type == engine::physics::ColliderType::Circle) {
        sf::Vector2f b_center{b_pos.x + 0.5f * b_size.x, b_pos.y + 0.5f * b_size.y};
        float b_radius = 0.5f * b_size.x;

        // 如果圆心在矩形内，直接相交
        if (a_rect.contains(b_center)) return true;

        // 否则，求矩形到圆心的最近点（clamp 圆心到矩形范围）
        float nearest_x = std::clamp(b_center.x, a_pos.x, a_pos.x + a_size.x);
        float nearest_y = std::clamp(b_center.y, a_pos.y, a_pos.y + a_size.y);
        return check_point_in_circle({nearest_x, nearest_y}, b_center, b_radius);
    }

    // Circle vs AABB (a Circle, b AABB)
    if (a_type == engine::physics::ColliderType::Circle && b_type == engine::physics::ColliderType::Aabb) {
        sf::Vector2f a_center{ a_pos.x + 0.5f * a_size.x, a_pos.y + 0.5f * a_size.y };
        float a_radius = 0.5f * a_size.x;

        if (b_rect.contains(a_center)) return true;

        float nearest_x = std::clamp(a_center.x, b_pos.x, b_pos.x + b_size.x);
        float nearest_y = std::clamp(a_center.y, b_pos.y, b_pos.y + b_size.y);
        return check_point_in_circle({nearest_x, nearest_y}, a_center, a_radius);
    }

    // 默认保险返回（不应该到这儿）
    return false;
}

bool check_circle_overlap(const sf::Vector2f& a_center, const float a_radius, const sf::Vector2f& b_center, const float b_radius) {
    sf::Vector2f delta = a_center - b_center;
    float radius_sum = a_radius + b_radius;
    return delta.lengthSquared() < radius_sum * radius_sum;
}

bool check_point_in_circle(const sf::Vector2f& point, const sf::Vector2f& center, const float radius) {
    sf::Vector2f delta = point - center;
    return delta.lengthSquared() < radius * radius;
}
} // namespace engine::physics::collision
