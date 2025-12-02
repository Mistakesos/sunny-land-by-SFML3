#include "ui_label.hpp"
#include "context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {
UILabel::UILabel(engine::render::Renderer& render
               , std::string_view text
               , std::string_view font_id
               , int font_size
               , const sf::Color& text_color
               , const sf::Vector2f& position)
    : UIElement{position}
    , render_{render}
    , text_{text}
    , font_id_{font_id}
    , font_size_{font_size}
    , text_color_{text_color} {
    // 获取文本渲染尺寸
    spdlog::trace("UILabel 构造完成");
}

void UILabel::render(engine::core::Context& context) {
    if (!visible_ || text_.empty()) return;
    auto& camera  = context.get_camera();

    render_.draw_ui_text(camera, text_, font_id_, font_size_, get_screen_position(), text_color_);

    // 渲染子元素（调用基类方法）
    UIElement::render(context);
}

void UILabel::set_text(std::string_view text) {
    text_ = text;
}

void UILabel::set_font_id(std::string_view font_id) {
    font_id_ = font_id;
}

void UILabel::set_font_size(int font_size) {
    font_size_ = font_size;
}

void UILabel::set_text_color(const sf::Color& text_color) {
    text_color_ = text_color;
    /* 颜色变化不影响尺寸 */
}
} // namespace engine::ui