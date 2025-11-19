#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <vector>
#include <optional>
#include <utility>

namespace engine::component {
    class PhysicsComponent;
    class TileLayerComponent;
    enum class TileType;
} // namespace engine::component

namespace engine::object {
    class GameObject;
} // namespace engine::object

namespace engine::physics {
class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    // 禁止拷贝和移动
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void register_component(engine::component::PhysicsComponent* component);       ///< @brief 注册物理组件
    void unregister_component(engine::component::PhysicsComponent* component);     ///< @brief 注销物理组件

    // 如果瓦片层需要进行碰撞检测则注册。（不需要则不必注册）
    void register_collision_layer(engine::component::TileLayerComponent* layer);   ///< @brief 注册用于碰撞检测的 TileLayerComponent
    void unregister_collision_layer(engine::component::TileLayerComponent* layer); ///< @brief 注销用于碰撞检测的 TileLayerComponent

    void update(sf::Time delta);        ///< @brief 核心循环：更新所有注册的物理组件的状态

    // 设置器/获取器
    void set_gravity(sf::Vector2f gravity) { gravity_ = std::move(gravity); }           ///< @brief 设置全局重力加速度
    const sf::Vector2f& get_gravity() const { return gravity_; }                        ///< @brief 获取当前的全局重力加速度
    void set_max_speed(sf::Vector2f max_speed) { max_speed_ = std::move(max_speed); }   ///< @brief 设置最大速度
    sf::Vector2f get_max_speed() const { return max_speed_; }                           ///< @brief 获取当前的最大速度
    void set_world_bounds(sf::FloatRect world_bounds) { world_bounds_ = std::move(world_bounds); }  ///< @brief 设置世界边界
    const std::optional<sf::FloatRect>& get_world_bounds() const { return world_bounds_; }          ///< @brief 获取世界边界
    ///< @brief 获取本帧检测到的所有GameObject碰撞对（此列表在每次update开始时清空）
    const std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>>& get_collision_pairs() { return collision_pairs_; }

private:
    void check_object_collisions();     ///< @brief 检测并处理对象之间的碰撞，并记录需要游戏逻辑处理的碰撞对
    /// @brief 检测并处理游戏对象和瓦片层之间的碰撞。
    void resolve_tile_collisions(engine::component::PhysicsComponent* pc, sf::Time delta);
    /// @brief 处理可移动物体与SOLID物体的碰撞。
    void resolve_solid_object_collisions(engine::object::GameObject* move_obj, engine::object::GameObject* solid_obj);
    void apply_world_bounds(engine::component::PhysicsComponent* pc);     ///< @brief 应用世界边界，限制物体移动范围

    /**
     * @brief 根据瓦片类型和指定宽度x坐标，计算瓦片上对应y坐标。
     * @param width 从瓦片左侧起算的宽度。
     * @param type 瓦片类型。
     * @param tile_size 瓦片尺寸。
     * @return 瓦片上对应高度（从瓦片下侧起算）。
     */
    float get_tile_height_at_width(float width, engine::component::TileType type, sf::Vector2f tile_size);

    std::vector<engine::component::PhysicsComponent*> components_;              ///< @brief 注册的物理组件容器，非拥有指针
    std::vector<engine::component::TileLayerComponent*> collision_tile_layers_; ///< @brief 注册的碰撞瓦片图层容器

    sf::Vector2f gravity_ = {0.f, 980.f};                           ///< @brief 默认重力值（像素/秒^2,相当于100像素对应现实1米）
    sf::Vector2f max_speed_ = {500.f, 500.f};                       ///< @brief 最大速度（像素/秒）
    std::optional<sf::FloatRect> world_bounds_;                     ///< @brief 世界边界，用于限制物体移动范围

    // @brief 储存本帧发生的 GameObject 碰撞对（每次 update 开始时清空）
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>> collision_pairs_;
};
} // namespace engine::physics