#include "game.hpp"
#include "action.hpp"
#include "time.hpp"
#include "config.hpp"
#include "resource_manager.hpp"
#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "game_scene.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "game_object.hpp"
#include "physics_engine.hpp"
#include "audio_player.hpp"
#include "context.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>

namespace engine::core {
Game::Game()
    : config_{std::make_unique<Config>("assets/config.json")}
    , window_{std::make_unique<sf::RenderWindow>(sf::VideoMode(config_->window_size_), config_->window_title_)}
    , time_{std::make_unique<Time>()}
    , resource_manager_{std::make_unique<engine::resource::ResourceManager>()}
    , input_manager_{std::make_unique<engine::input::InputManager>(window_.get(), config_.get())}
    , renderer_{std::make_unique<engine::render::Renderer>(window_.get(), resource_manager_.get())}
    , camera_{std::make_unique<engine::render::Camera>(window_.get())}
    , physics_engine_{std::make_unique<engine::physics::PhysicsEngine>()}
    , audio_player_{std::make_unique<engine::audio::AudioPlayer>(resource_manager_.get())}
    , context_{std::make_unique<engine::core::Context>(*input_manager_
                                                     , *renderer_
                                                     , *camera_
                                                     , *resource_manager_
                                                     , *physics_engine_
                                                     , *audio_player_)}
    , scene_manager_{std::make_unique<engine::scene::SceneManager>(*context_)} {
    // 创建第一个场景并压入栈
    auto scene = std::make_unique<game::scene::GameScene>("level_1", *context_, *scene_manager_);
    scene_manager_->request_push_scene(std::move(scene));
}

Game::~Game() = default;

void Game::run() {
    time_->set_target_fps(config_->target_fps_);
    
    while (window_->isOpen()) {

        input_manager_->update();

        handle_event();

        time_->accumulate_frame_time();
        while (time_->should_update()) {
            time_->consume_update_time();

            handle_event();

            update(time_->get_frame_duration());
        }

        render();
    }
}

void Game::handle_event() {
    input_manager_->process_event();

    scene_manager_->handle_input();
}

void Game::update(sf::Time delta) {
    scene_manager_->update(delta);
}

void Game::render() {
    window_->clear();

    scene_manager_->render();

    window_->display();
}
} // namespace engine::core