#include "menu_scene.hpp"
#include "title_scene.hpp"
#include "context.hpp"
#include "game_state.hpp"
#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "ui_manager.hpp"
#include "ui_panel.hpp"
#include "ui_label.hpp"
#include "ui_button.hpp"
#include "audio_player.hpp"
#include "session_data.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
MenuScene::MenuScene(engine::core::Context& context
                   , engine::scene::SceneManager& scene_manager
                   , std::shared_ptr<game::data::SessionData> session_data)
    : Scene{"MenuScene", context, scene_manager}
    , session_data_{std::move(session_data)} {
    context_.get_game_state().set_state(engine::core::State::Paused);
    if (!session_data_) {
        spdlog::error("菜单场景构造时 SessionData 为空。");
    }
    create_ui();

    spdlog::trace("menuScene 初始化完成");

    spdlog::trace("MenuScene 构造完成.");
}

void MenuScene::create_ui() {
    auto window_size = context_.get_game_state().get_logical_size();

    // "PAUSE"标签
    auto pause_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(), 
                                                             "PAUSE", 
                                                             "assets/fonts/VonwaonBitmap-16px.ttf", 
                                                             32);
    // 放在中间靠上的位置 
    auto size = pause_label->get_size();
    auto label_y = window_size.y * 0.2f;
    pause_label->set_position(sf::Vector2f((window_size.x - size.x) / 2.f, label_y));
    ui_manager_->add_element(std::move(pause_label));

    // --- 创建按钮 --- (4个按钮，设定好大小、间距)
    float button_width = 96.f; // 按钮稍微小一点
    float button_height = 32.f;
    float button_spacing = 10.f;
    float start_y = label_y + 80.f; // 从标签下方开始，增加间距
    float button_x = (window_size.x - button_width) / 2.f; // 水平居中

    // Resume Button
    auto resume_button = std::make_unique<engine::ui::UIButton>(context_,
                                                                "assets/textures/UI/buttons/Resume1.png", 
                                                                "assets/textures/UI/buttons/Resume2.png",  
                                                                "assets/textures/UI/buttons/Resume3.png",
                                                                sf::Vector2f{button_x, start_y},
                                                                sf::Vector2f{button_width, button_height},
                                                                [this]() { this->on_resume_clicked(); });
    ui_manager_->add_element(std::move(resume_button));

    // Save Button
    start_y += button_height + button_spacing;
    auto save_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Save1.png", 
                                                              "assets/textures/UI/buttons/Save2.png", 
                                                              "assets/textures/UI/buttons/Save3.png",
                                                              sf::Vector2f{button_x, start_y},
                                                              sf::Vector2f{button_width, button_height},
                                                              [this]() { this->on_save_clicked(); });
    ui_manager_->add_element(std::move(save_button));

    // Back Button
    start_y += button_height + button_spacing;
    auto back_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Back1.png", 
                                                              "assets/textures/UI/buttons/Back2.png", 
                                                              "assets/textures/UI/buttons/Back3.png",
                                                              sf::Vector2f{button_x, start_y},
                                                              sf::Vector2f{button_width, button_height},
                                                              [this]() { this->on_back_clicked(); });
    ui_manager_->add_element(std::move(back_button));

    // Quit Button
    start_y += button_height + button_spacing;
    auto quit_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Quit1.png", 
                                                              "assets/textures/UI/buttons/Quit2.png", 
                                                              "assets/textures/UI/buttons/Quit3.png",
                                                              sf::Vector2f{button_x, start_y},
                                                              sf::Vector2f{button_width, button_height},
                                                              [this]() { this->on_quit_clicked(); });
    ui_manager_->add_element(std::move(quit_button));
}

void MenuScene::handle_input() {
    // 先让 UIManager 处理交互
    Scene::handle_input();

    // 检查暂停键，允许按暂停键恢复游戏
    if (context_.get_input_manager().is_action_pressed(Action::Pause)) {
        spdlog::debug("在菜单场景中按下暂停键，正在恢复游戏...");
        scene_manager_.request_pop_scene();       // 弹出自身以恢复底层的GameScene
        context_.get_game_state().set_state(engine::core::State::Playing);
    }
}

// --- 按钮回调函数实现 ---

void MenuScene::on_resume_clicked() {
    spdlog::debug("继续游戏按钮被点击。");
    scene_manager_.request_pop_scene();           // 弹出当前场景
    context_.get_game_state().set_state(engine::core::State::Playing);
}

void MenuScene::on_save_clicked() {
    spdlog::debug("保存游戏按钮被点击。");
    if (session_data_ && session_data_->save_to_file("assets/save.json")) {
        spdlog::debug("菜单场景中成功保存游戏数据。");
    } else {
        spdlog::error("菜单场景中保存游戏数据失败。");
    }
}

void MenuScene::on_back_clicked() {
    spdlog::debug("返回按钮被点击。弹出菜单场景和游戏场景，返回标题界面。");
    // 直接替换为TitleScene
    scene_manager_.request_replace_scene(std::make_unique<TitleScene>(context_, scene_manager_, session_data_));
}

void MenuScene::on_quit_clicked() {
    spdlog::debug("退出按钮被点击。请求应用程序退出。");
    context_.get_input_manager().set_should_quit(true);     // 输入管理器设置退出标志
}
} // namespace game::scene