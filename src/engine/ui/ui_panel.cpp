#include "ui_panel.hpp"
#include "camera.hpp"
#include "context.hpp"
#include "render.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UIPanel::UIPanel(const sf::Vector2f& position, const sf::Vector2f& size, const std::optional<sf::Color>& background_color)
    : UIElement{position, size}
    , background_color_{background_color} {
    spdlog::trace("UIPanel 构造完成。");
}

void UIPanel::render(engine::core::Context& context) {
    if (!visible_) return;

    auto& camera = context.get_camera();

    if (background_color_) {
        context.get_renderer().draw_ui_filled_rect(camera, get_bounds(), background_color_.value());
    }

    UIElement::render(context); // 调用基类渲染方法(绘制子节点)
}
} // namespace engine::ui 