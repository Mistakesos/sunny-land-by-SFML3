#include "physics_engine.hpp"
#include "collision.hpp"
#include "game_object.hpp"
#include "physics_component.hpp"
#include "transform_component.hpp"
#include "tilelayer_component.hpp"
#include "collider_component.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::physics {
void PhysicsEngine::register_component(engine::component::PhysicsComponent* component) {
    components_.push_back(component);
    spdlog::trace("物理组件注册完成");
}

void PhysicsEngine::unregister_component(engine::component::PhysicsComponent* component) {
    // 使用 remove-erase 方法安全的移除指针
    auto it = std::remove(components_.begin(), components_.end(), component);
    components_.erase(it, components_.end());
    spdlog::trace("物理组件注销完成");
}

void PhysicsEngine::register_collision_layer(engine::component::TileLayerComponent* layer) {
    layer->set_physics_engine(this);    // 设置物理引擎
    collision_tile_layers_.push_back(layer);
    spdlog::trace("碰撞瓦片图层注册完成。");
}

void PhysicsEngine::unregister_collision_layer(engine::component::TileLayerComponent* layer) {
    auto it = std::remove(collision_tile_layers_.begin(), collision_tile_layers_.end(), layer);
    collision_tile_layers_.erase(it, collision_tile_layers_.end());
    spdlog::trace("碰撞瓦片图层注销完成。");
}

void PhysicsEngine::update(sf::Time delta) {
    // 每次开始时先清空碰撞对容器
    collision_pairs_.clear();
    
    for (auto* pc : components_) {
        if (!pc || !pc->is_enabled()) {
            continue;
        }

        // 应用重力
        if (pc->is_use_gravity()) {
            pc->add_force(gravity_ * pc->get_mass());
        }
        /* 还能添加其他力影响，比如风力、摩擦力、目前不考虑 */

        // 更新速度：v += a * dt, 其中 a = F / m
        pc->velocity_ += (pc->get_force() / pc->get_mass()) * delta.asSeconds();
        pc->clear_force();  // 清除当前帧的力

        // 处理瓦片层碰撞（速度和位置的更新移入此函数）
        resolve_tile_collisions(pc, delta);

        // 应用世界边界
        apply_world_bounds(pc);
    }
    // 处理对象间碰撞
    check_object_collisions();
}

void PhysicsEngine::check_object_collisions() {
    // 两层循环遍历所有包含物理组件的 GameObject
    for (size_t i = 0; i < components_.size(); ++i) {
        auto* pc_a = components_[i];
        if (!pc_a || !pc_a->is_enabled()) continue;
        auto* obj_a = pc_a->get_owner();
        if (!obj_a) continue;
        auto* cc_a = obj_a->get_component<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->is_active()) continue;

        for (size_t j = i + 1; j < components_.size(); ++j) {
            auto* pc_b = components_[j];
            if (!pc_b || !pc_b->is_enabled()) continue;
            auto* obj_b = pc_b->get_owner();
            if (!obj_b) continue;
            auto* cc_b = obj_b->get_component<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->is_active()) continue;
            /* --- 通过保护性测试后，正式执行逻辑 --- */

            if (collision::check_collision(*cc_a, *cc_b)) {
                // 如果是可移动物体与Solid物体碰撞，则直接处理位置变化，不用记录碰撞对
                if (obj_a->get_tag() != "solid" && obj_b->get_tag() == "solid") {
                    resolve_solid_object_collisions(obj_a, obj_b);
                } else if (obj_a->get_tag() == "solid" && obj_b->get_tag() != "solid") {
                    resolve_solid_object_collisions(obj_b, obj_a);
                } else {
                    // 记录碰撞对
                    collision_pairs_.emplace_back(obj_a, obj_b);
                }
            }
        }
    }
}

void PhysicsEngine::resolve_tile_collisions(engine::component::PhysicsComponent* pc, sf::Time delta) {
    // 检查组件是否有效
    auto* obj = pc->get_owner();
    if (!obj) return;
    auto* tc = obj->get_component<engine::component::TransformComponent>();
    auto* cc = obj->get_component<engine::component::ColliderComponent>();
    if (!tc || !cc || cc->is_trigger()) return;
    auto world_aabb = cc->get_world_aabb(); // 使用最小包围盒进行碰撞检测（简化）
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;
    if (world_aabb.size.x <= 0.f || world_aabb.size.y <= 0.f) return;
    // -- 检查结束, 正式开始处理 --

    constexpr float tolerance = 1.0f;               // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
    auto ds = pc->velocity_ * delta.asSeconds();    // 计算物体在delta_time内的位移
    auto new_obj_pos = obj_pos + ds;                // 计算物体在delta_time后的新位置

    if (!cc->is_active()) {  // 如果碰撞器未激活，直接让物体正常移动，然后返回。
        tc->translate(ds);
        // // 限制最大速度
        pc->velocity_.x = std::clamp(pc->velocity_.x, -max_speed_.x, max_speed_.x);
        pc->velocity_.y = std::clamp(pc->velocity_.y, -max_speed_.y, max_speed_.y);
        return;
    }

    // 遍历所有注册的碰撞瓦片层
    for (auto* layer : collision_tile_layers_) {
        if (!layer) continue;
        auto tile_size = layer->get_tile_size();
        // 轴分离碰撞检测：先检查X方向是否有碰撞 (y方向使用初始值obj_pos.y)
        if (ds.x > 0.f) {
            // 检查右侧碰撞，需要分别测试右上和右下角
            auto right_top_x = new_obj_pos.x + obj_size.x;
            auto tile_x = static_cast<int>(std::floor(right_top_x / tile_size.x));      // 获取x方向瓦片坐标
            // y方向坐标有两个，右上和右下
            auto tile_y = static_cast<int>(std::floor(obj_pos.y / tile_size.y));
            auto tile_type_top = layer->get_tile_type_at({tile_x, tile_y});             // 右上角瓦片类型
            auto tile_y_bottom = static_cast<int>(std::floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
            auto tile_type_bottom = layer->get_tile_type_at({tile_x, tile_y_bottom});   // 右下角瓦片类型

            if (tile_type_top == engine::component::TileType::Solid || tile_type_bottom == engine::component::TileType::Solid) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                new_obj_pos.x = tile_x * layer->get_tile_size().x - obj_size.x;
                pc->velocity_.x = 0.f;
            } else {
                // 检测右下角斜坡瓦片
                auto width_right = new_obj_pos.x + obj_size.x - tile_x * tile_size.x;
                auto height_right = get_tile_height_at_width(width_right, tile_type_bottom, static_cast<sf::Vector2f>(tile_size));
                if (height_right > 0.f) {
                    // 如果有碰撞（角点的世界y坐标 > 斜坡地面的世界y坐标）, 就让物体贴着斜坡表面
                    if (new_obj_pos.y > (tile_y_bottom + 1) * layer->get_tile_size().y - obj_size.y - height_right) {
                        new_obj_pos.y = (tile_y_bottom + 1) * layer->get_tile_size().y - obj_size.y - height_right;
                    }
                }
            }
        } else if (ds.x < 0.f) {
            // 检查左侧碰撞，需要分别测试左上和左下角
            auto left_top_x = new_obj_pos.x;
            auto tile_x = static_cast<int>(std::floor(left_top_x / tile_size.x));       // 获取x方向瓦片坐标
            // y方向坐标有两个，左上和左下
            auto tile_y = static_cast<int>(std::floor(obj_pos.y / tile_size.y));
            auto tile_type_top = layer->get_tile_type_at({tile_x, tile_y});             // 左上角瓦片类型
            auto tile_y_bottom = static_cast<int>(std::floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
            auto tile_type_bottom = layer->get_tile_type_at({tile_x, tile_y_bottom});   // 左下角瓦片类型

            if (tile_type_top == engine::component::TileType::Solid || tile_type_bottom == engine::component::TileType::Solid) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                new_obj_pos.x = (tile_x + 1) * layer->get_tile_size().x;
                pc->velocity_.x = 0.f;
            } else {
                // 检测左下角斜坡瓦片
                auto width_left = new_obj_pos.x - tile_x * tile_size.x;
                auto height_left = get_tile_height_at_width(width_left, tile_type_bottom, static_cast<sf::Vector2f>(tile_size));
                if (height_left > 0.f) {
                    if (new_obj_pos.y > (tile_y_bottom + 1) * layer->get_tile_size().y - obj_size.y - height_left) {
                        new_obj_pos.y = (tile_y_bottom + 1) * layer->get_tile_size().y - obj_size.y - height_left;
                    }
                }
            }
        }

        // 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值obj_pos.x)
        if (ds.y > 0.f) {
            // 检查底部碰撞，需要分别测试左下和右下角
            auto bottom_left_y = new_obj_pos.y + obj_size.y;
            auto tile_y = static_cast<int>(std::floor(bottom_left_y / tile_size.y));

            auto tile_x = static_cast<int>(std::floor(obj_pos.x / tile_size.x));
            auto tile_type_left = layer->get_tile_type_at({tile_x, tile_y});            // 左下角瓦片类型   
            auto tile_x_right = static_cast<int>(std::floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
            auto tile_type_right = layer->get_tile_type_at({tile_x_right, tile_y});     // 右下角瓦片类型

            if (tile_type_left == engine::component::TileType::Solid
                || tile_type_right == engine::component::TileType::Solid
                || tile_type_left == engine::component::TileType::Unisolid
                || tile_type_right == engine::component::TileType::Unisolid) {
                // 到达地面，速度归零，y方向移动到贴着地面的位置
                new_obj_pos.y = tile_y * layer->get_tile_size().y - obj_size.y;
                pc->velocity_.y = 0.f;
            } else {
                // 检测斜坡瓦片（下方两个角点都要检测）
                auto width_left = obj_pos.x - tile_x * tile_size.x;
                auto width_right = obj_pos.x + obj_size.x - tile_x_right * tile_size.x;
                auto height_left = get_tile_height_at_width(width_left, tile_type_left, static_cast<sf::Vector2f>(tile_size));
                auto height_right = get_tile_height_at_width(width_right, tile_type_right, static_cast<sf::Vector2f>(tile_size));
                auto height = std::max(height_left, height_right);  // 找到两个角点的最高点进行检测
                if (height > 0.f) {    // 说明至少有一个角点处于斜坡瓦片
                    if (new_obj_pos.y > (tile_y + 1) * layer->get_tile_size().y - obj_size.y - height) {
                        new_obj_pos.y = (tile_y + 1) * layer->get_tile_size().y - obj_size.y - height;
                        pc->velocity_.y = 0.f;     // 只有向下运动时才需要让 y 速度归零
                    }
                }
            }
        } else if (ds.y < 0.f) {
            // 检查顶部碰撞，需要分别测试左上和右上角
            auto top_left_y = new_obj_pos.y;
            auto tile_y = static_cast<int>(floor(top_left_y / tile_size.y));

            auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
            auto tile_type_left = layer->get_tile_type_at({tile_x, tile_y});        // 左上角瓦片类型
            auto tile_x_right = static_cast<int>(floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
            auto tile_type_right = layer->get_tile_type_at({tile_x_right, tile_y});     // 右上角瓦片类型

            if (tile_type_left == engine::component::TileType::Solid || tile_type_right == engine::component::TileType::Solid) {
                // 撞到天花板！速度归零，y方向移动到贴着天花板的位置
                new_obj_pos.y = (tile_y + 1) * layer->get_tile_size().y;
                pc->velocity_.y = 0.f;
            }
        }
    }
    // 更新物体位置，并限制最大速度
    tc->translate(new_obj_pos - obj_pos);   // 使用translate方法，避免直接设置位置，因为碰撞箱可能有偏移
    pc->velocity_.x = std::clamp(pc->velocity_.x, -max_speed_.x, max_speed_.x);
    pc->velocity_.y = std::clamp(pc->velocity_.y, -max_speed_.y, max_speed_.y);
}

void PhysicsEngine::resolve_solid_object_collisions(engine::object::GameObject* move_obj, engine::object::GameObject* solid_obj) {
    // 进入此函数前，已经检查了各个组件的有效性，因此直接进行计算
    auto* move_tc = move_obj->get_component<engine::component::TransformComponent>();
    auto* move_pc = move_obj->get_component<engine::component::PhysicsComponent>();
    auto* move_cc = move_obj->get_component<engine::component::ColliderComponent>();
    auto* solid_cc = solid_obj->get_component<engine::component::ColliderComponent>();

    // 这里只能获取期望位置，无法获取当前帧初始位置，因此无法进行轴分离碰撞检测
    /* 未来可以进行重构，让这里可以获取初始位置。但是我们展示另外一种处理方法 */
    auto move_aabb = move_cc->get_world_aabb();
    auto solid_aabb = solid_cc->get_world_aabb();

    auto intersection = move_aabb.findIntersection(solid_aabb);
    if (!intersection) return;

    auto overlap = intersection->size;

    // --- 使用最小平移向量解决碰撞问题 ---
    auto move_center = move_aabb.position + move_aabb.size / 2.f;
    auto solid_center = solid_aabb.position + solid_aabb.size / 2.f;

    if (overlap.x < overlap.y) {    // 如果重叠部分在x方向上更小，则认为碰撞发生在x方向上（推出x方向平移向量最小）
        if (move_center.x < solid_center.x) {
            // 移动物体在左边，让它贴着右边SOLID物体（相当于向左移出重叠部分），y方向正常移动
            move_tc->translate(sf::Vector2f(-overlap.x, 0.f));
            // 如果速度为正(向右移动)，则归零 （if判断不可少，否则可能出现错误吸附）
            if (move_pc->velocity_.x > 0.f) {
                move_pc->velocity_.x = 0.f;
            }
        } else {
            // 移动物体在右边，让它贴着左边SOLID物体（相当于向右移出重叠部分），y方向正常移动
            move_tc->translate(sf::Vector2f(overlap.x, 0.f));
            if (move_pc->velocity_.x < 0.f) {
                move_pc->velocity_.x = 0.f;
            }
        }
    } else {                        // 重叠部分在y方向上更小，则认为碰撞发生在y方向上（推出y方向平移向量最小）
        if (move_center.y < solid_center.y) {
            // 移动物体在上面，让它贴着下面SOLID物体（相当于向上移出重叠部分），x方向正常移动
            move_tc->translate(sf::Vector2f(0.f, -overlap.y));
            if (move_pc->velocity_.y > 0.f) {
                move_pc->velocity_.y = 0.f;
            }
        } else {
            // 移动物体在下面，让它贴着上面SOLID物体（相当于向下移出重叠部分），x方向正常移动
            move_tc->translate(sf::Vector2f(0.f, overlap.y));
            if (move_pc->velocity_.y < 0.f) {
                move_pc->velocity_.y = 0.f;
            }
        }
    }
}

void PhysicsEngine::apply_world_bounds(engine::component::PhysicsComponent* pc) {
    if (!pc || !world_bounds_) return;

    // 只限定左、上、右边界，不限定下边界，以碰撞盒作为判断依据
    auto* obj = pc->get_owner();
    auto* cc = obj->get_component<engine::component::ColliderComponent>();
    auto* tc = obj->get_component<engine::component::TransformComponent>();
    auto world_aabb = cc->get_world_aabb();
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;

    // 检查左边界
    if (obj_pos.x < world_bounds_->position.x) {
        pc->velocity_.x = 0.f;
        obj_pos.x = world_bounds_->position.x;
    }
    // 检查上边界
    if (obj_pos.y < world_bounds_->position.y) {
        pc->velocity_.y = 0.f;
        obj_pos.y = world_bounds_->position.y;
    }
    // 检查右边界
    if (obj_pos.x + obj_size.x > world_bounds_->position.x + world_bounds_->size.x) {
        pc->velocity_.x = 0.f;
        obj_pos.x = world_bounds_->position.x + world_bounds_->size.x - obj_size.x;
    }
    // 更新物体位置(使用translate方法，新位置 - 旧位置)
    tc->translate(obj_pos - world_aabb.position);
}

float PhysicsEngine::get_tile_height_at_width(float width, engine::component::TileType type, sf::Vector2f tile_size) {
    auto rel_x = std::clamp(width / tile_size.x, 0.f, 1.f);
    switch (type) {
        case engine::component::TileType::Slope_0_1:        // 左0  右1
            return rel_x * tile_size.y;
        case engine::component::TileType::Slope_0_2:        // 左0  右1/2
            return rel_x * tile_size.y * 0.5f;
        case engine::component::TileType::Slope_2_1:        // 左1/2右1
            return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
        case engine::component::TileType::Slope_1_0:        // 左1  右0
            return (1.0f - rel_x) * tile_size.y;
        case engine::component::TileType::Slope_2_0:        // 左1/2右0
            return (1.0f - rel_x) * tile_size.y * 0.5f;
        case engine::component::TileType::Slope_1_2:        // 左1  右1/2
            return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
        default:
            return 0.f;   // 默认返回0，表示没有斜坡
    }
}
} // namespace engine::physics