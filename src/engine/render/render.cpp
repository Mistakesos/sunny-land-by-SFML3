#include "render.hpp"

#include "resource_manager.hpp"
#include "camera.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

engine::render::Renderer::Renderer(sf::RenderWindow* window, engine::resource::ResourceManager* resource_manager)
    : window_obs_{window}
    , resourec_manager_obs_{resource_manager} {
    spdlog::trace("构造 Renderer...");
    if (!window_obs_) {
        throw std::runtime_error("Renderer 构造失败：提供的 window 指针为空");
    }
    if (!resourec_manager_obs_) {
        throw std::runtime_error("ResourceManager 构造失败：提供的 ResourceManager 指针为空");
    }
    spdlog::trace("Renderer 构造成功");
}

engine::render::Renderer::~Renderer() = default;

void engine::render::Renderer::clear_frame() {
    window_obs_->clear(sf::Color::Black);
}

void engine::render::Renderer::display_frame() {
    window_obs_->display();
}

void engine::render::Renderer::draw_sprite(const Camera& camera, sf::Sprite& sprite) {
    window_obs_->setView(camera.get_world_view());
    window_obs_->draw(sprite);
}

// void engine::render::Renderer::draw_parallax(const Camera& camera, sf::Sprite& sprite,
//                                              const sf::Vector2f& scroll_factor, sf::Vector2<bool> repeat, const sf::Vector2f& scale) {
//     // 应用相机变换
//     auto position = sprite.getPosition();

//     // 设置位置、缩放
//     sprite.setPosition(position);
//     sprite.setScale(scale);

//     sf::IntRect original_texture_rect = sprite.getTextureRect();

//     // 如果需要重复，调整纹理矩形
//     if (repeat.x || repeat.y) {
//         sf::Vector2f viewport_size = camera.get_world_viewport_size();

//         // 计算缩放后的精灵尺寸
//         sf::Vector2f scaled_sprite_size = {
//             original_texture_rect.size.x * scale.x,
//             original_texture_rect.size.y * scale.y
//         };

//         // 修复问题1：计算相机偏移量，确保向左移动时也能正确重复
//         sf::Vector2f camera_offset = {
//             std::fmod(camera.get_center().x * scroll_factor.x, scaled_sprite_size.x),
//             std::fmod(camera.get_center().y * scroll_factor.y, scaled_sprite_size.y)
//         };

//         // 计算需要覆盖的区域（考虑两个方向）
//         sf::IntRect texture_rect = original_texture_rect;

//         if (repeat.x) {
//             // 修复问题2：安全计算重复次数，避免整数溢出
//             int repeat_count_x = static_cast<int>(std::ceil(viewport_size.x / scaled_sprite_size.x)) + 2; // +2 确保覆盖边界
//             texture_rect.size.x = original_texture_rect.size.x * repeat_count_x;

//             // 调整纹理矩形的left，确保向左移动时也能正确显示
//             texture_rect.position.x = original_texture_rect.position.x - static_cast<int>(camera_offset.x);
//         }

//         if (repeat.y) {
//             int repeat_count_y = static_cast<int>(std::ceil(viewport_size.y / scaled_sprite_size.y)) + 2;
//             texture_rect.size.y = original_texture_rect.size.y * repeat_count_y;

//             texture_rect.position.y = original_texture_rect.position.y - static_cast<int>(camera_offset.y);
//         }

//         // 设置纹理重复模式
//         sprite.setTextureRect(texture_rect);

//         // 调整精灵位置以补偿纹理偏移
//         sprite.setPosition({
//             position.x - (repeat.x ? camera_offset.x : 0),
//             position.y - (repeat.y ? camera_offset.y : 0)
//         });
//     }

//     // 绘制
//     window_obs_->setView(camera.get_world_view());
//     window_obs_->draw(sprite);
// }

static float positive_fmod(float x, float m) {
    // 把浮点 x 模 m 映射到 [0, m)
    if (m == 0.f) return 0.f;
    float r = std::fmod(x, m);
    if (r < 0) r += std::abs(m);
    return r;
}

void engine::render::Renderer::draw_parallax(
    const Camera& camera,
    sf::Sprite& sprite,
    const sf::Vector2f& scroll_factor,
    sf::Vector2<bool> repeat,
    const sf::Vector2f& scale
) {
    // 源纹理区域（一个 tile 的像素大小）
    sf::IntRect src = sprite.getTextureRect();
    if (src.size.x <= 0 || src.size.y <= 0) return;

    // 视图（世界视图）信息
    const sf::View& view = camera.get_world_view();
    sf::Vector2f view_center = camera.get_world_view_center();
    sf::Vector2f view_size = camera.get_world_view_size(); // 通常 = view.getSize()

    // 世界中这个 parallax 层的“原始位置”（通常是你给 sprite 的 position）
    sf::Vector2f layer_pos = sprite.getPosition();

    // 缩放后一个 tile 的世界尺寸（注意：这是世界坐标下 tile 的大小）
    sf::Vector2f tile_world_size = {
        static_cast<float>(src.size.x) * scale.x,
        static_cast<float>(src.size.y) * scale.y
    };

    // 先设置缩放（用于绘制单个 tile）
    sprite.setScale(scale);

    // 计算相机左/上边界（世界坐标）
    sf::Vector2f view_left_top = view_center - view_size * 0.5f;
    sf::Vector2f view_right_bottom = view_center + view_size * 0.5f;

    // 计算 parallax 引起的偏移：相机中心乘以 scroll_factor 表示“相机看向层的偏移”
    // 直觉：当 camera 向右移动时，parallax 层应该移动 scroll_factor * camera_center 的量
    sf::Vector2f parallax_offset = { view_center.x * scroll_factor.x, view_center.y * scroll_factor.y };

    // 我们要在“层坐标系”里找到从哪里开始绘制（确保覆盖整个视口，并在左/上加一格 buffer）
    // 计算视口在“层空间”的左上坐标（即把相机视口映射到这一层上）
    sf::Vector2f view_left_top_in_layer = view_left_top - parallax_offset;
    sf::Vector2f view_right_bottom_in_layer = view_right_bottom - parallax_offset;

    // 计算第一个要绘制的 tile 的左上角 x,y（在世界坐标系中）
    // 我们使用 floor((view_left - layer_pos) / tile_size) * tile_size + layer_pos
    // 再向左 / 向上退一步以覆盖边界（防止留缝）
    auto compute_start = [&](float view_left, float layer_origin, float tile_size) -> float {
        if (tile_size <= 0.f) return layer_origin;
        float relative = (view_left - layer_origin) / tile_size;
        float index = std::floor(relative);
        float start = layer_origin + index * tile_size;
        // 退一格作为 buffer（这是教程中 -tile 的效果）
        start -= tile_size;
        return start;
    };

    float start_x = compute_start(view_left_top_in_layer.x, layer_pos.x, tile_world_size.x);
    float start_y = compute_start(view_left_top_in_layer.y, layer_pos.y, tile_world_size.y);

    // 计算结束边界（在 layer 空间中）
    float end_x = view_right_bottom_in_layer.x + tile_world_size.x; // 加一个 tile 保险
    float end_y = view_right_bottom_in_layer.y + tile_world_size.y;

    // 切回世界视图（渲染时使用 world view）
    window_obs_->setView(view);

    // 如果不需要重复某一方向，直接只绘制原始位置的那一块（可能仍需考虑偏移）
    if (!repeat.x && !repeat.y) {
        // 直接把 sprite 放置在 layer_pos - parallax_offset（因为相机有 parallax）
        sprite.setPosition(layer_pos - parallax_offset);
        window_obs_->draw(sprite);
        return;
    }

    // 循环绘制 tile（x,y）
    for (float y = start_y; y < end_y; y += tile_world_size.y) {
        // 如果不重复 Y，则只绘制一行，y = layer_pos.y - parallax_offset.y
        if (!repeat.y) y = layer_pos.y - parallax_offset.y;

        for (float x = start_x; x < end_x; x += tile_world_size.x) {
            // 如果不重复 X，则只绘制一列，x = layer_pos.x - parallax_offset.x
            if (!repeat.x) x = layer_pos.x - parallax_offset.x;

            // 计算该 tile 在“世界坐标”中的左上角
            // 注意：start_x/start_y 是 layer 空间（layer 原点未做 parallax），
            // 把它转换回世界坐标：tile_world_pos + parallax_offset
            // 但我们绘制位置应为 world 坐标：tileLayerPos - parallax_offset
            // 等价于：tile_world_pos - parallax_offset
            sf::Vector2f tile_world_pos = { x, y };                     // 在layer坐标（已经基于 layer_pos）
            sf::Vector2f draw_pos = tile_world_pos + parallax_offset;  // 转为世界坐标系中的绘制位置

            // 但这里我们之前的 math 是：start_* 基于 layer_pos，且 view_left_top_in_layer = view_left - parallax_offset
            // 为避免混淆，直接把 tile 的世界坐标设为：
            sf::Vector2f tile_world_draw_pos = { x + parallax_offset.x, y + parallax_offset.y };

            // 最终设置精灵位置并绘制
            sprite.setPosition(tile_world_draw_pos);
            window_obs_->draw(sprite);

            if (!repeat.x) break;
        }

        if (!repeat.y) break;
    }
}


void engine::render::Renderer::draw_ui_sprite(const Camera& camera, sf::Sprite& sprite) {
    window_obs_->setView(camera.get_ui_view());
    window_obs_->draw(sprite);
}

void engine::render::Renderer::draw_rect(const sf::FloatRect& rect, sf::Color color) {
    sf::RectangleShape shape;
    shape.setPosition({rect.position.x, rect.position.y});
    shape.setSize({rect.size.x, rect.size.y});
    shape.setFillColor(color);
    window_obs_->draw(shape);
}