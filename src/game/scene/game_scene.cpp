#include "game_scene.hpp"
#include "resource_manager.hpp"
#include "context.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "transform_component.hpp"
#include "sprite_component.hpp"
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
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handle_input() {
    Scene::handle_input();
    test_camera();
}

void GameScene::creat_test_object() {
    spdlog::trace("在 GameScene 中创建 test_object...");
    auto test_object = std::make_unique<engine::object::GameObject>("test_object");

    test_object->add_component<engine::component::TransformComponent>(sf::Vector2f(84.f, 84.f));
    test_object->add_component<engine::component::SpriteComponent>(*context_.get_resource_manager().get_texture("assets/textures/Props/big-crate.png"));

    add_game_object(std::move(test_object));
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
} // namespace game::scene