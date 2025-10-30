#include "game.hpp"
#include "action.hpp"
#include "time.hpp"
#include "config.hpp"
#include "resource_manager.hpp"
#include "input_manager.hpp"
#include "render.hpp"
#include "camera.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>

engine::core::Game::Game()
    : config_{std::make_unique<engine::core::Config>("../assets/config.json")}
    , window_{std::make_unique<sf::RenderWindow>(sf::VideoMode(config_->window_size_), config_->window_title_)}
    , time_{std::make_unique<Time>()}
    , resource_manager_{std::make_unique<engine::resource::ResourceManager>()}
    , input_manager_{std::make_unique<engine::input::InputManager>(window_.get(), config_.get())}
    , renderer_{std::make_unique<engine::render::Renderer>(window_.get(), resource_manager_.get())}
    , camera_{std::make_unique<engine::render::Camera>(window_.get())} {
}

engine::core::Game::~Game() = default;

void engine::core::Game::run() {
    test_resource_manager();

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
        // spdlog::info("delta_time: {}", time_->get_frame_duration().asSeconds());

        render();
    }
}

void engine::core::Game::handle_event() {
    // while (std::optional event = window_->pollEvent()) {
    //     if (event->is<sf::Event::Closed>()) {
    //         window_->close();
    //     }
    // }

    test_input_manager();
}

void engine::core::Game::update(const sf::Time& delta) {
    test_camera();
}

void engine::core::Game::render() {
    window_->clear();

    test_renderer();

    window_->display();
}

void engine::core::Game::test_resource_manager() {
    resource_manager_->get_texture("../assets/textures/Actors/eagle-attack.png");
    resource_manager_->get_font("../assets/fonts/VonwaonBitmap-16px.ttf");
    resource_manager_->get_sound("../assets/audio/button_click.wav");

    resource_manager_->unload_texture("../assets/textures/Actors/eagle-attack.png");
    resource_manager_->unload_font("../assets/fonts/VonwaonBitmap-16px.ttf");
    resource_manager_->unload_sound("../assets/audio/button_click.wav");
}

void engine::core::Game::test_renderer() {
    sf::Sprite spr_world(*resource_manager_->get_texture("../assets/textures/Actors/frog.png"));
    spr_world.setPosition({200.f, 200.f});

    sf::Sprite spr_ui(*resource_manager_->get_texture("../assets/textures/UI/buttons/Start1.png"));
    spr_ui.setPosition({100.f, 100.f});

    auto& tex_back = *resource_manager_->get_texture("../assets/textures/Layers/back.png");
    tex_back.setRepeated(true);
    sf::Sprite spr_parallax(tex_back);
    spr_parallax.setPosition({100.f, 100.f});
    spr_parallax.setTextureRect({{0, 0}, {768, 240}});

    static float rotation = 0.f;
    rotation += 0.1f;

    spr_world.setOrigin(spr_world.getLocalBounds().getCenter());
    spr_world.setRotation(sf::degrees(rotation));
    auto pos = spr_world.getPosition();

    renderer_->draw_parallax(*camera_, spr_parallax, {0.5f, 0.5f}, {true, false});
    renderer_->draw_sprite(*camera_, spr_world);
    renderer_->draw_ui_sprite(*camera_, spr_ui);
}

void engine::core::Game::test_camera() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) camera_->move({0.f, -1.f});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) camera_->move({0.f, 1.f});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) camera_->move({-1.f, 0.f});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) camera_->move({1.f, 0.f});
}

void engine::core::Game::test_input_manager() {
    std::vector<Action> actions = {
        Action::MoveUp,
        Action::MoveDown,
        Action::MoveLeft,
        Action::MoveRight,
        Action::Jump,
        Action::Attack,
        Action::Pause,
    };

    for (const auto& action : actions) {
        if (input_manager_->is_action_pressed(action)) {
            spdlog::info("{} 按下", static_cast<int>(action));
        }
        if (input_manager_->is_action_released(action)) {
            spdlog::info("{} 抬起", static_cast<int>(action));
        }
        if (input_manager_->is_action_held(action)) {
            spdlog::info("{} 长按", static_cast<int>(action));
        }
    }
}
