#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace engine::physics {
/**
 * @brief 定义不同类型的碰撞器
 */
enum class ColliderType {
    None,
    Aabb,
    Circle
};

/**
 * @brief 碰撞器的抽象基类
 * 所有具体的碰撞器都应继承此类
 */
class Collider {
public:
    Collider() = default;
    virtual ~Collider() = default;
    virtual ColliderType get_type() const = 0;  ///< @brief 获取碰撞器类型

    void set_aabb_size(const sf::Vector2f& size) { aabb_size_ = size; } ///< @brief 设置最小包围盒的尺寸（宽度和高度）
    const sf::Vector2f& get_aabb_size() const { return aabb_size_; }    ///< @brief 获取最小包围盒的尺寸（高度和宽度）

protected:
    sf::Vector2f aabb_size_ = {0.f, 0.f};   ///< @brief 获取碰撞器的类型
};

/**
 * @brief 轴对齐包围盒（Axis-Aligned Bounding Box)碰撞器
 */
class AABBCollider final : public Collider {
public:
    /**
     * @brief 构造函数
     * @param size 包围盒的宽度和高度
     */
    explicit AABBCollider(const sf::Vector2f& size)
        : size_{size} { 
        set_aabb_size(size);
    }
    ~AABBCollider() override = default;

    // --- Getter and Setters
    ColliderType get_type() const override { return ColliderType::Aabb; }
    const sf::Vector2f& get_size() const { return size_; }
    void set_size(const sf::Vector2f& size) { size_ = size; }


private:
    sf::Vector2f size_ = {0.f, 0.f};    ///< @brief 包围盒的尺寸（和aabb_size_相同）
};

/**
 * @brief 圆形碰撞器
 */
class CircleCollider final : public Collider {
public:
    /**
     * @brief 构造函数
     * @param radius 圆的半径
     */
    explicit CircleCollider(float radius)
        : radius_(radius) {
        set_aabb_size(sf::Vector2f(radius * 2.f, radius * 2.f));
    }
    ~CircleCollider() override = default;

    // --- Getters and Setters ---
    ColliderType get_type() const override { return ColliderType::Circle; }
    float get_radius() const { return radius_; }
    void set_raduius(float radius) { radius_ = radius; }
    
private:
    float radius_ = 0.f;    ///< @brief 圆的半径
};
} // namespace engine::physics