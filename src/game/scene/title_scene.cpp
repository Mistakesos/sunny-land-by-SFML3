#include "title_scene.hpp"
#include "context.hpp"
#include "resource_manager.hpp"
#include "game_state.hpp"
#include "camera.hpp"
#include "input_manager.hpp"
#include "ui_manager.hpp"
#include "ui_panel.hpp"
#include "ui_image.hpp"
#include "ui_button.hpp"
#include "ui_label.hpp"
#include "audio_player.hpp"
#include "level_loader.hpp"
#include "scene_manager.hpp"
#include "session_data.hpp"
#include "game_scene.hpp"
#include "help_scene.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
TitleScene::TitleScene(engine::core::Context& context
                     , engine::scene::SceneManager& scene_manager
                     , std::shared_ptr<game::data::SessionData> session_data)
    : engine::scene::Scene{"TitleScene", context, scene_manager}
    , session_data_{std::move(session_data)} {
    context_.get_game_state().set_state(engine::core::State::Title);
    context_.get_camera().set_world_view_center(context_.get_camera().get_world_view_size() / 2.f);     // 涉及到切换场景，将位置重置为初始位置
    context_.get_camera().set_limit_bounds(std::nullopt);       // 解除边界限制，让相机能正常移动

    if (!session_data_) {
        spdlog::warn("TitleScene 接收到空的 SessionData，创建一个默认的 SessionData");
        session_data_ = std::make_shared<game::data::SessionData>();
    }
    session_data_->sync_high_score("assets/save.json");      // 更新最高分

    // 加载背景地图
    engine::scene::LevelLoader level_loader(context);
    if (!level_loader.load_level("assets/maps/level_0.tmj", *this)) {
        spdlog::error("加载背景失败");
        return;
    }
    
    // 创建 UI 元素
    create_ui();

    spdlog::trace("TitleScene 初始化完成.");

    spdlog::trace("TitleScene 创建");
}

// 创建 UI 界面元素
void TitleScene::create_ui() {
    spdlog::trace("创建 TitleScene UI...");
    auto window_size = context_.get_game_state().get_logical_size();
    
    // 设置背景音乐
    context_.get_audio_player().play_music("assets/audio/platformer_level03_loop.ogg");

    // 设置音量
    context_.get_audio_player().set_music_volume(20.f);  // 设置背景音乐音量为20%
    context_.get_audio_player().set_sound_volume(20.f);  // 设置音效音量为20%

    // 创建标题图片 (假设不知道大小)
    auto title_image = std::make_unique<engine::ui::UIImage>(
        *context_.get_resource_manager().get_texture("assets/textures/UI/title-screen.png")
    );
    auto size = title_image->get_sprite().getGlobalBounds().size;
    title_image->set_size(size * 2.f);      // 放大为2倍

    // 水平居中
    auto title_pos = (window_size - title_image->get_size()) / 2.f - sf::Vector2f(0.f, 50.f);
    title_image->set_position(title_pos);
    ui_manager_->add_element(std::move(title_image));

    // --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
    float button_width = 96.f;
    float button_height = 32.f;
    float button_spacing = 20.f;
    int num_buttons = 4;

    // 计算面板总宽度
    float panel_width = num_buttons * button_width + (num_buttons - 1) * button_spacing;
    float panel_height = button_height;

    // 计算面板位置使其居中
    float panel_x = (window_size.x - panel_width) / 2.f;
    float panel_y = window_size.y * 0.65f;  // 垂直位置中间靠下

    auto button_panel = std::make_unique<engine::ui::UIPanel>(
        sf::Vector2f(panel_x, panel_y),
        sf::Vector2f(panel_width, panel_height)
    );

    // --- 创建按钮并添加到 UIPanel (位置是相对于 UIPanel 的 0,0) ---
    sf::Vector2f current_button_pos = sf::Vector2f(0.f, 0.f);
    sf::Vector2f button_size = sf::Vector2f(button_width, button_height);

    // Start Button
    auto start_button = std::make_unique<engine::ui::UIButton>(context_, 
                                                              "assets/textures/UI/buttons/Start1.png",
                                                              "assets/textures/UI/buttons/Start2.png",
                                                              "assets/textures/UI/buttons/Start3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->on_start_game_click();});
    button_panel->add_child(std::move(start_button));

    // Load Button
    current_button_pos.x += button_width + button_spacing;
    auto load_button = std::make_unique<engine::ui::UIButton>(context_, 
                                                              "assets/textures/UI/buttons/Load1.png",
                                                              "assets/textures/UI/buttons/Load2.png",
                                                              "assets/textures/UI/buttons/Load3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->on_load_game_click();});
    button_panel->add_child(std::move(load_button));

    // Helps Button
    current_button_pos.x += button_width + button_spacing;
    auto helps_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Helps1.png",
                                                              "assets/textures/UI/buttons/Helps2.png",
                                                              "assets/textures/UI/buttons/Helps3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->on_helps_click();});
    button_panel->add_child(std::move(helps_button));

    // Quit Button
    current_button_pos.x += button_width + button_spacing;
    auto quit_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Quit1.png",
                                                              "assets/textures/UI/buttons/Quit2.png",
                                                              "assets/textures/UI/buttons/Quit3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->on_quit_click();});
    button_panel->add_child(std::move(quit_button));

    // 将 UIPanel 添加到UI管理器
    ui_manager_->add_element(std::move(button_panel));

    // 创建 Credits 标签
    auto credits_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(),
                                                               "Sunny_Land Credits: LanJing - 2025",
                                                               "assets/fonts/VonwaonBitmap-16px.ttf",
                                                               16,
                                                               sf::Color(204, 204, 204));
    credits_label->set_position(sf::Vector2f{(window_size.x - credits_label->get_size().x) / 2.f, 
                                          window_size.y - credits_label->get_size().y - 10.f});
    ui_manager_->add_element(std::move(credits_label));

    spdlog::trace("TitleScene UI 创建完成.");
}

// 更新场景逻辑
void TitleScene::update(sf::Time delta) {
    Scene::update(delta);

    // 相机自动向右移动
    context_.get_camera().move(sf::Vector2f(delta.asSeconds() * 100.f, 0.f));
}

// --- 按钮回调实现 --- //

void TitleScene::on_start_game_click() {
    spdlog::debug("开始游戏按钮被点击。");
    if (session_data_) {
        session_data_->reset();
    }
    scene_manager_.request_replace_scene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}

void TitleScene::on_load_game_click() {
    spdlog::debug("加载游戏按钮被点击。");
    if (!session_data_) {
        spdlog::error("游戏状态为空，无法加载。");
        return;
    }

    if (session_data_->load_from_file("assets/save.json")) {
        spdlog::debug("保存文件加载成功。开始游戏...");
        scene_manager_.request_replace_scene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
    } else {
        spdlog::warn("加载保存文件失败。");
    }
}

void TitleScene::on_helps_click() {
    spdlog::debug("帮助按钮被点击。");
    scene_manager_.request_push_scene(std::make_unique<HelpsScene>(context_, scene_manager_));
}

void TitleScene::on_quit_click() {
    spdlog::debug("退出按钮被点击。");
    session_data_->sync_high_score("assets/save.json");   // 退出前先同步最高分
    context_.get_input_manager().set_should_quit(true);
}
} // namespace game::scenes 