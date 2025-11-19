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
#include "level_loader.hpp"
#include "input_manager.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{name, context, scene_manager} {
    // 加载关卡（level_loader通常加载完成后即可销毁，因此不存为成员变量
    engine::scene::LevelLoader level_loader(context);
    if (level_loader.load_level("assets/maps/level_1.tmj", *this));

    // 注册 “main”层到物理引擎
    auto* main_layer = find_game_object_by_name("main");
    if (main_layer) {
        auto* tile_layer = main_layer->get_component<engine::component::TileLayerComponent>();
        if (tile_layer) {
            context.get_physics_engine().register_collision_layer(tile_layer);
            spdlog::info("注册‘main’层到物理引擎");
        }
    }

    player_obs_ = find_game_object_by_name("player");
    if (!player_obs_) {
        spdlog::error("未找到玩家对象");
    }
    
    spdlog::trace("GameScene 构造成功");
}

GameScene::~GameScene() = default;

void GameScene::update(sf::Time delta) {
    Scene::update(delta);
    test_collision_pairs();
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handle_input() {
    Scene::handle_input();
    test_player();
}

void GameScene::test_camera() {
    auto& camera = context_.get_camera();
    auto& input_manager = context_.get_input_manager();
    if (input_manager.is_action_held(Action::MoveUp)) camera.move({0.f, -1.f});
    if (input_manager.is_action_held(Action::MoveDown)) camera.move({0.f, 1.f});
    if (input_manager.is_action_held(Action::MoveLeft)) camera.move({-1.f, 0.f});
    if (input_manager.is_action_held(Action::MoveRight)) camera.move({1.f, 0.f});
}

void GameScene::test_player() {
    if (!player_obs_) return;
    auto& input_manager = context_.get_input_manager();
    auto* pc = player_obs_->get_component<engine::component::PhysicsComponent>();
    if (!pc) return;

    if (input_manager.is_action_held(Action::MoveLeft)) {
        pc->velocity_.x = -100.f;
    } else {
        pc->velocity_.x *= 0.9f;
    }

    if (input_manager.is_action_held(Action::MoveRight)) {
        pc->velocity_.x = 100.f;
    } else {
        pc->velocity_.x *= 0.9f;
    }

    if (input_manager.is_action_pressed(Action::Jump)) {
        pc->velocity_.y = -400.f;
    }
}

void GameScene::test_collision_pairs() {
    auto collision_pairs = context_.get_physics_engine().get_collision_pairs();
    for (auto& pair : collision_pairs) {
        spdlog::info("键值对：{} 和 {}", pair.first->get_name(), pair.second->get_name());
    }
}
} // namespace game::scene