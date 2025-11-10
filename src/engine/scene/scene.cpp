#include "scene.hpp"
#include "context.hpp"
#include "game_object.hpp"
#include "scene_manager.hpp"
#include "physics_engine.hpp"
#include "context.hpp"
#include <spdlog/spdlog.h>

namespace engine::scene {
Scene::Scene(std::string_view name, engine::core::Context& context, SceneManager& scene_manager)
    : scene_name_(name)
    , context_{context}
    , scene_manager_{scene_manager} {
    is_initialized_ = true;         // 子类应该最后调用父类的init方法
    spdlog::trace("场景 ‘{}’ 初始化完成", scene_name_);
}

Scene::~Scene() = default;

void Scene::update(sf::Time delta) {
    if (!is_initialized_) return;

    // 先更新物理引擎
    context_.get_physics_engine().update(delta);
    
    bool need_remove = false;  // 设定一个标志，用于判断是否需要移除对象

    // 更新所有游戏对象，先略过需要移除的对象
    for (auto& obj : game_objects_) {
        if (obj && !obj->is_need_remove()) {
            obj->update(delta, context_);
        } else {
            need_remove = true;
            if (!obj) spdlog::warn("尝试更新一个空的游戏对象指针。");
        }
    }

    if (need_remove) {
        // 使用C++20新添加的erase_if删除需要移除的对象，比使用erase - remove_if更简洁
        // NOTE: 用此语句则没有机会调用clean方法，因此要在update中先调用clean方法
        std::erase_if(game_objects_, [](const std::unique_ptr<engine::object::GameObject>& obj) {
            return !obj || obj->is_need_remove();
        });
    }

    process_pending_additions();      // 处理待添加（延时添加）的游戏对象
}

void Scene::render() {
    if (!is_initialized_) return;
    // 渲染所有游戏对象
    for (const auto& obj : game_objects_) {
        if (obj) obj->render(context_);
    }
}

void Scene::handle_input() {
    if (!is_initialized_) return;

    // 遍历所有游戏对象，略过需要移除的对象
    for (auto& obj : game_objects_) {
        if (obj && !obj->is_need_remove()) {
            obj->handle_input(context_);
        }
    }
}

void Scene::add_game_object(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (game_object) game_objects_.push_back(std::move(game_object));
    else spdlog::warn("尝试向场景 '{}' 添加空游戏对象。", scene_name_);
}

void Scene::safe_add_game_object(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (game_object) pending_additions_.push_back(std::move(game_object));
    else spdlog::warn("尝试向场景 '{}' 添加空游戏对象。", scene_name_);
}

void Scene::remove_game_object(engine::object::GameObject* game_object_ptr) {
    if (!game_object_ptr) {
        spdlog::warn("尝试从场景 '{}' 中移除一个空的游戏对象指针。", scene_name_);
        return;
    }

    // erase-remove 移除法不可用，因为智能指针与裸指针无法比较
    // 需要使用 std::remove_if 和 lambda 表达式自定义比较的方式
    auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
                             [game_object_ptr](const std::unique_ptr<engine::object::GameObject>& p) {
                                 return p.get() == game_object_ptr;    // 比较裸指针是否相等（自定义比较方式）
                             });

    if (it != game_objects_.end()) {
        game_objects_.erase(it, game_objects_.end());   // 删除从it到末尾的元素（最后一个元素）
        spdlog::trace("从场景 '{}' 中移除游戏对象。", scene_name_);
    } else {
        spdlog::warn("游戏对象指针未找到在场景 '{}' 中。", scene_name_);
    }
}

void Scene::safe_remove_game_object(engine::object::GameObject* game_object_ptr) {
    game_object_ptr->set_need_remove(true);
}

const std::vector<std::unique_ptr<engine::object::GameObject>>& Scene::get_game_objects() const {
    return game_objects_;
}

engine::object::GameObject* Scene::find_game_object_by_name(std::string_view name) const {
        // 找到第一个符合条件的游戏对象就返回
    for (const auto& obj : game_objects_) {
        if (obj && obj->get_name() == name) {
            return obj.get();
        }
    }
    return nullptr;
}

void Scene::process_pending_additions() {
    // 处理待添加的游戏对象
    for (auto& game_object : pending_additions_) {
        add_game_object(std::move(game_object));
    }
    pending_additions_.clear();
}
} // namespace engine::scene