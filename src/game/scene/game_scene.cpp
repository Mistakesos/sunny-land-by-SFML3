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
#include "level_loader.hpp"
#include "input_manager.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene{name, context, scene_manager} {
    engine::scene::LevelLoader level_loader(context);
    if (level_loader.load_level("assets/maps/level_1.tmj", *this));
    creat_test_object();
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
    test_object();
}

void GameScene::creat_test_object() {
    spdlog::trace("在 GameScene 中创建 test_object...");
    auto test_object = std::make_unique<engine::object::GameObject>("test_object");
    test_object_obs_ = test_object.get();

    // 添加组件
    test_object->add_component<engine::component::TransformComponent>(sf::Vector2f{100.f, 100.f});
    test_object->add_component<engine::component::SpriteComponent>(*context_.get_resource_manager().get_texture("assets/textures/Props/big-crate.png"));
    test_object->add_component<engine::component::PhysicsComponent>(&context_.get_physics_engine());
    test_object->add_component<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(sf::Vector2f{32.f, 32.f}));
    // 将其添加到场景
    add_game_object(std::move(test_object));

    // 添加组件
    auto test_object2 = std::make_unique<engine::object::GameObject>("test_object2");
    test_object2->add_component<engine::component::TransformComponent>(sf::Vector2f{50.f, 50.f});
    test_object2->add_component<engine::component::SpriteComponent>(*context_.get_resource_manager().get_texture("assets/textures/Props/big-crate.png"));
    test_object2->add_component<engine::component::PhysicsComponent>(&context_.get_physics_engine(), false);
    test_object2->add_component<engine::component::ColliderComponent>(std::make_unique<engine::physics::CircleCollider>(16.f));
    // 将其添加到场景
    add_game_object(std::move(test_object2));

    spdlog::trace("test_object 创建并添加到 GameScene 中");
}

void GameScene::test_camera() {
    auto& camera = context_.get_camera();
    auto& input_manager = context_.get_input_manager();
    if (input_manager.is_action_held(Action::MoveUp)) camera.move({0.f, -1.f});
    if (input_manager.is_action_held(Action::MoveDown)) camera.move({0.f, 1.f});
    if (input_manager.is_action_held(Action::MoveLeft)) camera.move({-1.f, 0.f});
    if (input_manager.is_action_held(Action::MoveRight)) camera.move({1.f, 0.f});
}

void GameScene::test_object() {
    if (!test_object_obs_) return;
    auto& input_manager = context_.get_input_manager();

    if (input_manager.is_action_held(Action::MoveLeft)) {
        test_object_obs_->get_component<engine::component::TransformComponent>()->translate({-0.1f, 0.f});
    }
    if (input_manager.is_action_held(Action::MoveRight)) {
        test_object_obs_->get_component<engine::component::TransformComponent>()->translate({0.1f, 0.f});
    }
    if (input_manager.is_action_pressed(Action::Jump)) {
        test_object_obs_->get_component<engine::component::PhysicsComponent>()->set_velocity({0.f, -400.f});
    }
}

void GameScene::test_collision_pairs() {
    auto collision_pairs = context_.get_physics_engine().get_collision_pairs();
    for (auto& pair : collision_pairs) {
        spdlog::info("键值对：{} 和 {}", pair.first->get_name(), pair.second->get_name());
    }
}
} // namespace game::scene