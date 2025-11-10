#include "tilelayer_component.hpp"
#include "context.hpp"
#include "render.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
TileLayerComponent::TileLayerComponent(engine::object::GameObject* owner, sf::Vector2i tile_size, sf::Vector2i map_size, std::vector<TileInfo>&& tiles)
    : Component{owner}
    , tile_size_(tile_size)
    , map_size_(map_size)
    , tiles_(std::move(tiles)) {
    if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y)) {
        spdlog::error("TileLayerComponent: 地图尺寸与提供的瓦片向量大小不匹配。瓦片数据将被清除。");
        tiles_.clear();
        map_size_ = {0, 0};
    }
    
    spdlog::trace("TileLayerComponent 构造完成");
}

TileLayerComponent::~TileLayerComponent() = default;

const TileInfo* TileLayerComponent::get_tile_info_at(sf::Vector2i pos) const {
    if (pos.x < 0 || pos.x >= map_size_.x || pos.y < 0 || pos.y >= map_size_.y) {
        spdlog::warn("TileLayerComponent: 瓦片坐标越界: ({}, {})", pos.x, pos.y);
        return nullptr;
    }
    size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);
    // 瓦片索引不能越界
    if (index < tiles_.size()) {
        return &tiles_[index];
    }
    spdlog::warn("TileLayerComponent: 瓦片索引越界: {}", index);
    return nullptr;
}

TileType TileLayerComponent::get_tile_type_at(sf::Vector2i pos) const {
    const TileInfo* info = get_tile_info_at(pos);
    return info ? info->type : TileType::Empty;
}

TileType TileLayerComponent::get_tile_type_at_world_pos(const sf::Vector2f& world_pos) const {
    sf::Vector2f relative_pos = world_pos - offset_;

    int tile_x = static_cast<int>(std::floor(relative_pos.x / tile_size_.x));
    int tile_y = static_cast<int>(std::floor(relative_pos.y / tile_size_.y));

    return get_tile_type_at(sf::Vector2i{tile_x, tile_y});
}

void TileLayerComponent::render(engine::core::Context& context) {
    if (tile_size_.x <= 0 || tile_size_.y <= 0) {
        spdlog::warn("除以零或无效尺寸！");
        return; // 防止除以零或无效尺寸
    }

    // 遍历所有瓦片
    for (int y = 0; y < map_size_.y; y++) {
        for (int x = 0; x < map_size_.x; x++) {
            size_t index = static_cast<size_t>(y) * map_size_.x + x;
            // 检查索引有效性以及瓦片是否需要渲染
            if (index < tiles_.size() && tiles_[index].type != TileType::Empty) {
                auto& tile_info = tiles_[index];
                // 计算该瓦片在世界中的左上角位置（sprite默认左上角）
                sf::Vector2f tile_left_top_pos = {
                    offset_.x + static_cast<float>(x) * tile_size_.x,
                    offset_.y + static_cast<float>(y) * tile_size_.y
                };
                // 但如果图片的大小与瓦片的大小不一致，需要调整 y 坐标 （瓦片层的对齐点是左下角）
                if (static_cast<int>(tile_info.sprite.getGlobalBounds().size.y) != tile_size_.y) {
                    tile_left_top_pos.y -= (tile_info.sprite.getGlobalBounds().size.y - static_cast<float>(tile_size_.y));
                }
                // 实际应用变换后的位置
                tile_info.sprite.setPosition(tile_left_top_pos);
                // 执行绘制
                context.get_renderer().draw_sprite(context.get_camera(), tile_info.sprite);
            }
        }
    }
}
} // namespace engine::component