#include "game_scene.hpp"
#include "resource_manager.hpp"
#include "physics_engine.hpp"
#include "context.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "transform_component.hpp"
#include "sprite_component.hpp"
#include "physics_component.hpp"
#include "collider_component.hpp"
#include "tilelayer_component.hpp"
#include "player_component.hpp"
#include "level_loader.hpp"
#include "input_manager.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{name, context, scene_manager} {
    // 加载关卡（level_loader通常加载完成后即可销毁，因此不存为成员变量
    engine::scene::LevelLoader level_loader(context_);
    if (!level_loader.load_level("assets/maps/level_1.tmj", *this)) {
        spdlog::error("关卡加载失败！");
    }

    // 注册 “main”层到物理引擎
    auto* main_layer = find_game_object_by_name("main");
    if (main_layer) {
        auto* tile_layer = main_layer->get_component<engine::component::TileLayerComponent>();
        if (tile_layer) {
            context_.get_physics_engine().register_collision_layer(tile_layer);
            spdlog::info("注册‘main’层到物理引擎");
        }
    }

    // 获取玩家对象指针（非拥有）
    player_obs_ = find_game_object_by_name("player");
    if (!player_obs_) {
        spdlog::error("未找到玩家对象");
    }

    // 添加PlayerComponent到玩家对象
    auto* player_component = player_obs_->add_component<game::component::PlayerComponent>();
    if (!player_component) {
        spdlog::error("无法找到 PlayerComponent 到玩家对象");
    }

    // 相机跟随玩家
    auto* player_transform = player_obs_->get_component<engine::component::TransformComponent>();
    if (player_transform) {
        context_.get_camera().set_target(player_transform);
    }

    // 设置相机边界
    auto world_size = main_layer->get_component<engine::component::TileLayerComponent>()->get_world_size();
    context_.get_camera().set_limit_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));

    // 设置世界边界
    context_.get_physics_engine().set_world_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));
    
    spdlog::trace("GameScene 构造成功");
}

GameScene::~GameScene() = default;

void GameScene::update(sf::Time delta) {
    Scene::update(delta);
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handle_input() {
    Scene::handle_input();
}
} // namespace game::scene