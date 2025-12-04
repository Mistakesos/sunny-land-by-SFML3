#include "help_scene.hpp"
#include "context.hpp"
#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "resource_manager.hpp"
#include "game_state.hpp"
#include "ui_manager.hpp"
#include "ui_image.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
HelpsScene::HelpsScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : engine::scene::Scene{"HelpsScene", context, scene_manager} {
    auto window_size = context_.get_game_state().get_logical_size();

    // 创建帮助图片 UIImage （让它覆盖整个屏幕）
    auto help_image = std::make_unique<engine::ui::UIImage>(
        *context.get_resource_manager().get_texture("assets/textures/UI/instructions.png"),
        sf::Vector2f(0.f, 0.f),
        window_size
    );

    ui_manager_->add_element(std::move(help_image));

    spdlog::trace("HelpsScene 初始化完成.");

    spdlog::trace("HelpsScene 创建.");
}

void HelpsScene::handle_input() {
    // 检测是否按下鼠标左键
    if (context_.get_input_manager().is_action_pressed(Action::MouseLeftClick)
        || context_.get_input_manager().is_action_pressed(Action::Pause)) {
        spdlog::debug("鼠标左键被按下, 退出 HelpsScene.");
        scene_manager_.request_pop_scene();
    }
}
} // namespace game::scene