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
#include "animation_component.hpp"
#include "level_loader.hpp"
#include "input_manager.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{name, context, scene_manager} {
    if (!init_level()) {
        spdlog::error("关卡初始化失败！");
        return;
    }
    if (!init_player()) {
        spdlog::error("玩家初始化失败！");
        return;
    }
    if (!init_enemy_and_item()) {
        spdlog::error("敌人和道具初始化失败！");
        return;
    }
    
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

bool GameScene::init_level() {
     // 加载关卡
    engine::scene::LevelLoader level_loader(context_);
    if (!level_loader.load_level("assets/maps/level_1.tmj", *this)) {
        spdlog::error("关卡加载失败！");
        return false;
    }

    // 注册 "main"层到物理引擎
    auto* main_layer = find_game_object_by_name("main");
    if (!main_layer) {
        spdlog::error("未找到'main'层");
        return false;
    }

    auto* tile_layer = main_layer->get_component<engine::component::TileLayerComponent>();
    if (!tile_layer) {
        spdlog::error("'main'层没有TileLayerComponent");
        return false;
    }

    context_.get_physics_engine().register_collision_layer(tile_layer);
    spdlog::info("注册'main'层到物理引擎");

    // 设置相机边界和世界边界
    auto world_size = tile_layer->get_world_size();
    context_.get_camera().set_limit_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));
    context_.get_physics_engine().set_world_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));

    return true;
}

bool GameScene::init_player() {
    // 获取玩家对象指针
    player_obs_ = find_game_object_by_name("player");
    if (!player_obs_) {
        spdlog::error("未找到玩家对象");
        return false;
    }

    // 添加PlayerComponent到玩家对象
    auto* player_component = player_obs_->add_component<game::component::PlayerComponent>();
    if (!player_component) {
        spdlog::error("无法添加PlayerComponent到玩家对象");
        return false;
    }

    // 相机跟随玩家
    auto* player_transform = player_obs_->get_component<engine::component::TransformComponent>();
    if (player_transform) {
        context_.get_camera().set_target(player_transform);
    } else {
        spdlog::warn("玩家对象没有TransformComponent，相机无法跟随");
    }

    return true;
}

bool GameScene::init_enemy_and_item() {
    bool success = true;
    for (auto& game_object : game_objects_){
        if (game_object->get_name() == "eagle"){
            if (auto* ac = game_object->get_component<engine::component::AnimationComponent>(); ac){
                ac->play_animation("fly");
            } else {
                spdlog::error("Eagle对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->get_name() == "frog"){
            if (auto* ac = game_object->get_component<engine::component::AnimationComponent>(); ac){
                ac->play_animation("idle");
            } else {
                spdlog::error("Frog对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->get_name() == "opossum"){
            if (auto* ac = game_object->get_component<engine::component::AnimationComponent>(); ac){
                ac->play_animation("walk");
            } else {
                spdlog::error("Opossum对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->get_tag() == "item"){
            if (auto* ac = game_object->get_component<engine::component::AnimationComponent>(); ac){
                ac->play_animation("idle");
            } else {
                spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
    }

    return success;
}
} // namespace game::scene