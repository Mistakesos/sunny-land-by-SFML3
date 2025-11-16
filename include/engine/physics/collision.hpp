#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace engine::component {
    class ColliderComponent;
} // namespace engine::component

namespace engine::physics::collision {
/**
 * @brief 检查两个碰撞器组件是否重叠
 * @param a 第一个碰撞器组件
 * @param b 第二个碰撞器组件
 * @return 如果有重叠返回ture，否则false
 */
bool check_collision(const engine::component::ColliderComponent& a, const engine::component::ColliderComponent& b);

/**
 * @brief 检查两个圆形是否重叠。
 * 
 * @param a_center 第一个圆的中心。
 * @param a_radius 第一个圆的半径。
 * @param b_center 第二个圆的中心。
 * @param b_radius 第二个圆的半径。
 * @return true 如果两个圆重叠，否则为 false。
 */
bool check_circle_overlap(const sf::Vector2f& a_center, const float a_radius, const sf::Vector2f& b_center, const float b_radius);

/**
 * @brief 检查一个点是否在圆内。
 * 
 * @param point 要检查的点。
 * @param center 圆的中心。
 * @param radius 圆的半径。
 * @return true 如果点在圆内，否则为 false。
 */
bool check_point_in_circle(const sf::Vector2f& point, const sf::Vector2f& center, const float radius);
} // namespace engine::physics::collision