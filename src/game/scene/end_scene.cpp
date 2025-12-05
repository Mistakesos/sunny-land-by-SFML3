#include "end_scene.hpp"
#include "context.hpp"
#include "game_scene.hpp"
#include "game_state.hpp"
#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "session_data.hpp"
#include "title_scene.hpp"
#include "ui_button.hpp"
#include "ui_label.hpp"
#include "ui_manager.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {
EndScene::EndScene(engine::core::Context& context
                 , engine::scene::SceneManager& scene_manager
                 , std::shared_ptr<game::data::SessionData> session_data)
    : engine::scene::Scene{"EndScene", context, scene_manager}
    , session_data_{std::move(session_data)} {
    if (!session_data_) {
        spdlog::error("错误：结束场景收到了空的游戏数据！");
    }
    // 设置游戏状态为 GameOver
    context_.get_game_state().set_state(engine::core::State::GameOver);

    create_ui();

    spdlog::info("EndScene 初始化完成。");

    spdlog::trace("EndScene (胜利：{}) 创建.", session_data_->get_is_win() ? "是" : "否");
}

void EndScene::create_ui() {
    auto window_size = context_.get_game_state().get_logical_size();
    auto is_win = session_data_->get_is_win();

    // --- 主文字标签 ---
    std::string main_message = is_win ? "YOU WIN! CONGRATS!" : "YOU LOSE! TRY AGAIN!";
    // 赢了是绿色，输了是红色
    sf::Color message_color = is_win 
        ? sf::Color::Green
        : sf::Color::Red;

    auto main_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(),
                                                            main_message,
                                                            "assets/fonts/VonwaonBitmap-16px.ttf",
                                                            48,
                                                            message_color);
    // 标签居中
    sf::Vector2f label_size = main_label->get_size();
    sf::Vector2f main_label_pos = {(window_size.x - label_size.x) / 2.f, window_size.y * 0.3f};
    main_label->set_position(main_label_pos);
    ui_manager_->add_element(std::move(main_label));


    // --- 得分标签 ---
    int current_score = session_data_->get_current_score();
    int high_score = session_data_->get_high_score();
    sf::Color score_color = sf::Color::White;
    int score_font_size = 24;

    // 当前得分
    std::string score_text = "Score: " + std::to_string(current_score);
    auto score_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(),
                                                             score_text,
                                                             "assets/fonts/VonwaonBitmap-16px.ttf",
                                                             score_font_size,
                                                             score_color);
    sf::Vector2f score_label_size = score_label->get_size();
    // x方向居中，y方向在主标签下方20像素
    sf::Vector2f score_label_pos = {(window_size.x - score_label_size.x) / 2.f, main_label_pos.y + label_size.y + 20.f};
    score_label->set_position(score_label_pos);
    ui_manager_->add_element(std::move(score_label));

    // --- 最高分 ---
    std::string high_score_text = "High Score: " + std::to_string(high_score);
    auto high_score_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(),
                                                                  high_score_text,
                                                                  "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                  score_font_size,
                                                                  score_color);
    sf::Vector2f high_score_label_size = high_score_label->get_size();
    // x方向居中，y方向在当前得分下方10像素
    sf::Vector2f high_score_label_pos = {(window_size.x - high_score_label_size.x) / 2.f, score_label_pos.y + score_label_size.y + 10.f};
    high_score_label->set_position(high_score_label_pos);
    ui_manager_->add_element(std::move(high_score_label));

    // --- UI按钮 ---
    sf::Vector2f button_size = {120.f, 40.f};        // 让按钮更大一点
    float button_spacing = 20.f;
    float total_button_width = button_size.x * 2 + button_spacing;

    // 按钮放在右下角，与边缘间隔30像素
    float buttons_x = window_size.x - total_button_width - 30.f;
    float buttons_y = window_size.y - button_size.y - 30.f;

    // Back Button
    auto back_button = std::make_unique<engine::ui::UIButton>(context_,
                                                              "assets/textures/UI/buttons/Back1.png",
                                                              "assets/textures/UI/buttons/Back2.png",
                                                              "assets/textures/UI/buttons/Back3.png",
                                                              sf::Vector2f{buttons_x, buttons_y},
                                                              button_size,
                                                              [this]() { this->on_back_click(); });
    ui_manager_->add_element(std::move(back_button));

    // Restart Button
    buttons_x += button_size.x + button_spacing;
    auto restart_button = std::make_unique<engine::ui::UIButton>(context_,
                                                                 "assets/textures/UI/buttons/Restart1.png",
                                                                 "assets/textures/UI/buttons/Restart2.png",
                                                                 "assets/textures/UI/buttons/Restart3.png",
                                                                 sf::Vector2f{buttons_x, buttons_y},
                                                                 button_size,
                                                                 [this]() { this->on_restart_click(); });
    ui_manager_->add_element(std::move(restart_button));
}

void EndScene::on_back_click() {
    spdlog::info("返回按钮被点击。");
    scene_manager_.request_replace_scene(std::make_unique<TitleScene>(context_, scene_manager_, session_data_));
}

void EndScene::on_restart_click() {
    spdlog::info("重新开始按钮被点击。");
    // 重新开始游戏
    session_data_->reset();
    scene_manager_.request_replace_scene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}
} // namespace game::scene