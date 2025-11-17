#include "level_loader.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"
#include "game_object.hpp"
#include "component.hpp"
#include "transform_component.hpp"
#include "sprite_component.hpp"
#include "tilelayer_component.hpp"
#include "parallax_component.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <filesystem>
#include <optional>

namespace engine::scene {
LevelLoader::LevelLoader(engine::core::Context& context)
    : context_{context} {
}

LevelLoader::~LevelLoader() = default;

bool LevelLoader::load_level(std::string_view level_path, Scene& scene) {
    map_path_ = level_path;
    auto path = std::filesystem::path(level_path);
    // 1、加载 josn 文件
    std::ifstream file(path);
        if (!file.is_open()) {
        spdlog::error("无法打开关卡文件: {}", level_path);
        return false;
    }

    // 2、解析 json 数据
    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("解析 JSON 数据失败: {}", e.what());
        return false;
    }

    // 3. 获取基本地图信息 (名称、地图尺寸、瓦片尺寸)
    map_path_ = level_path;
    map_size_ = sf::Vector2i(json_data.value("width", 0), json_data.value("height", 0));
    tile_size_ = sf::Vector2i(json_data.value("tilewidth", 0), json_data.value("tileheight", 0));

    // 4. 加载 tileset 数据
    if (json_data.contains("tilesets") && json_data["tilesets"].is_array()) {
        for (const auto& tileset_json : json_data["tilesets"]) {
            if (!tileset_json.contains("source") || !tileset_json["source"].is_string() ||
                !tileset_json.contains("firstgid") || !tileset_json["firstgid"].is_number_integer()) {
                spdlog::error("tilesets 对象中缺少有效 'source' 或 'firstgid' 字段。");
                continue;
            }
            auto tileset_path = resolve_path(tileset_json["source"].get<std::string>(), map_path_);  // 支持隐式转换，可以省略.get<T>()方法，
            auto first_gid = tileset_json["firstgid"];
            load_tileset(tileset_path, first_gid);
        }
    }

    // 5、加载图层数据
    if (!json_data.contains("layers") || !json_data["layers"].is_array()) {       // 地图文件中必须有 layers 数组
        spdlog::error("地图文件 '{}' 中缺少或无效的 'layers' 数组。", level_path);
        return false;
    }
    for (const auto& layer_json : json_data["layers"]) {
        // 获取各图层对象中的类型（type）字段
        std::string layer_type = layer_json.value("type", "none");
        if (!layer_json.value("visible", true)) {
            spdlog::info("图层 '{}' 不可见，跳过加载。", layer_json.value("name", "Unnamed"));
            continue;
        }

        // 根据图层类型决定加载方法
        if (layer_type == "imagelayer") {
            load_image_layer(layer_json, scene);
        } else if (layer_type == "tilelayer") {
            load_tile_layer(layer_json, scene);
        } else if (layer_type == "objectgroup") {
            load_object_layer(layer_json, scene);
        } else {
            spdlog::warn("不支持的图层类型: {}", layer_type);
        }
    }

    spdlog::info("关卡加载完成: {}", level_path);
    return true;
}

void LevelLoader::load_image_layer(const nlohmann::json& layer_json, Scene& scene) {
    // 获取纹理相对路径 （会自动处理'\/'符号）
    std::string image_path = layer_json.value("image", "");     // json.value()返回的是一个临时对象，需要赋值才能保存，
                                                                // 不能用std::string_view
    if (image_path.empty()) {
        spdlog::error("图层 '{}' 缺少 'image' 属性。", layer_json.value("name", "Unnamed"));
        return;
    }
    auto texture_id = resolve_path(image_path, map_path_);

    // 获取图层偏移量（json中没有则代表未设置，给默认值即可）
    const sf::Vector2f offset = sf::Vector2f(layer_json.value("offsetx", 0.f), layer_json.value("offsety", 0.f));

    // 获取视差因子及重复标志
    const sf::Vector2f scroll_factor = sf::Vector2f(layer_json.value("parallaxx", 1.f), layer_json.value("parallaxy", 1.f));
    const sf::Vector2<bool> repeat = sf::Vector2<bool>(layer_json.value("repeatx", false), layer_json.value("repeaty", false));

    // 获取图层名称
    std::string layer_name = layer_json.value("name", "Unnamed");

    /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */

    // 创建游戏对象
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    // 依次添加Transform，Parallax组件
    game_object->add_component<engine::component::TransformComponent>(offset);
    game_object->add_component<engine::component::ParallaxComponent>(*context_.get_resource_manager().get_texture(texture_id), scroll_factor, repeat);
    // 添加到场景中
    scene.add_game_object(std::move(game_object));
    spdlog::info("加载图层: '{}' 完成", layer_name);
}

void LevelLoader::load_tile_layer(const nlohmann::json& layer_json, Scene& scene) {
    if (!layer_json.contains("data") || !layer_json["data"].is_array()) {
        spdlog::error("图层 '{}' 缺少 'data' 属性。", layer_json.value("name", "Unnamed"));
        return;
    }
    // 准备 TileInfo Vector (瓦片数量 = 地图宽度 * 地图高度)
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(map_size_.x * map_size_.y);

    // 获取图层数据 (瓦片 ID 列表)
    const auto& data = layer_json["data"];

    // 根据gid获取必要信息，并依次填充 TileInfo Vector
    for (const auto& gid : data) {
        tiles.push_back(get_tile_info_by_gid(gid));
    }

    // 获取图层名称
    std::string layer_name = layer_json.value("name", "Unnamed");
    // 创建游戏对象
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    // 添加Tilelayer组件
    game_object->add_component<engine::component::TileLayerComponent>(tile_size_, map_size_, std::move(tiles));
    // 添加到场景中
    scene.add_game_object(std::move(game_object));
    spdlog::info("加载瓦片图层: '{}' 完成", layer_name);

}

void LevelLoader::load_object_layer(const nlohmann::json& layer_json, Scene& scene) {
    if (!layer_json.contains("objects") || !layer_json["objects"].is_array()) {
        spdlog::error("对象图层 '{}' 缺少 'objects' 属性。", layer_json.value("name", "Unnamed"));
        return;
    }
    // 获取对象数据
    const auto& objects = layer_json["objects"];
    // 遍历对象数据
    for (const auto& object : objects) {
        // 获取对象gid
        auto gid = object.value("gid", 0);
        if (gid == 0) {
            // 如果gid为0（即不存在），则代表自己绘制的形状（可能是碰撞盒、触发器，未来按需处理）
        } else {
            // --- 根据gid获取必要信息，每个gid对应一个游戏对象 ---
            auto tile_info = get_tile_info_by_gid(gid);

            // 获取Transform相关信息
            auto position = sf::Vector2f(object.value("x", 0.f), object.value("y", 0.f));
            auto dst_size = sf::Vector2f(object.value("width", 0.f), object.value("height", 0.f));
            position = sf::Vector2f(position.x, position.y - dst_size.y);   // 实际position需要进行调整（左下角到左上角）

            auto rotation = sf::degrees(object.value("rotation", 0.f));
            auto src_size = tile_info.sprite.getGlobalBounds().size;
            auto scale = dst_size.componentWiseDiv(src_size);

            // 获取对象名称
            const std::string& object_name = object.value("name", "Unnamed");

            // 创建游戏对象并添加组件
            auto game_object = std::make_unique<engine::object::GameObject>(object_name);
            game_object->add_component<engine::component::TransformComponent>(position, scale, rotation);
            game_object->add_component<engine::component::SpriteComponent>(std::move(tile_info.sprite));

            // 添加到场景中
            scene.add_game_object(std::move(game_object));
            spdlog::info("加载对象：{} 完成", object_name);
        }
    }
}

engine::component::TileType LevelLoader::get_tile_type(const nlohmann::json& tile_json) {
    if (tile_json.contains("properties")) {
        auto& properties = tile_json["properties"];
        for (auto& property : properties) {
            if (property.contains("name") && property["name"] == "solid") {
                auto is_solid = property.value("value", false);
                return is_solid ? engine::component::TileType::Solid : engine::component::TileType::Normal;
            }
            // TODO: 未来添加更多自定义属性处理逻辑，当前只针对solid
        }
    }
    return engine::component::TileType::Normal;
}

engine::component::TileType LevelLoader::get_tile_type_by_id(const nlohmann::json& tileset_json, int local_id) {
    if (tileset_json.contains("tiles")) {
        auto& tiles = tileset_json["tiles"];
        for (auto& tile : tiles) {
            if (tile.contains("id") && tile["id"] == local_id) {
                return get_tile_type(tile);
            }
        }
    }
    return engine::component::TileType::Normal;
}

engine::component::TileInfo LevelLoader::get_tile_info_by_gid(int gid) {
    sf::Sprite default_sprite(*context_.get_resource_manager().get_texture("assets/textures/Props/big-crate.png"));
    auto default_tile_info = engine::component::TileInfo(default_sprite);

    if (gid == 0) {
        return default_tile_info;
    }

    // upper_bound：查找tileset_data_中键大于 gid 的第一个元素，返回迭代器
    auto tileset_it = tileset_data_.upper_bound(gid);
    if (tileset_it == tileset_data_.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return default_tile_info;
    }
    --tileset_it;  // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tileset = tileset_it->second;
    auto local_id = gid - tileset_it->first;        // 计算瓦片在图块集中的局部ID
    std::string file_path = tileset.value("file_path", "");       // 获取图块集文件路径
    if (file_path.empty()) {
        spdlog::error("Tileset 文件 '{}' 缺少 'file_path' 属性。", tileset_it->first);
        return default_tile_info;
    }

    // 图块集分为两种情况，需要分别考虑
    if (tileset.contains("image")) {    // 这是单一图片的情况
        // 获取图片路径
        auto texture_id = resolve_path(tileset["image"].get<std::string>(), file_path);
        // 计算瓦片在图片网格中的坐标
        auto coordinate_x = local_id % tileset["columns"].get<int>();
        auto coordinate_y = local_id / tileset["columns"].get<int>();
        // 根据坐标确定源矩形
        sf::IntRect texture_rect = {
            {coordinate_x * tile_size_.x,
            coordinate_y * tile_size_.y},
            {tile_size_.x,
            tile_size_.y}
        };
        sf::Sprite sprite{*context_.get_resource_manager().get_texture(texture_id), texture_rect};
        auto tile_type = get_tile_type_by_id(tileset, local_id);
        return engine::component::TileInfo(sprite, tile_type);
    } else {   // 这是多图片的情况
        if (!tileset.contains("tiles")) {   // 没有tiles字段的话不符合数据格式要求，直接返回空的瓦片信息
            spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tileset_it->first);
            return default_tile_info;
        }
        // 遍历tiles数组，根据id查找对应的瓦片
        const auto& tiles_json = tileset["tiles"];
        for (const auto& tile_json : tiles_json) {
            auto tile_id = tile_json.value("id", 0);
            if (tile_id == local_id) {   // 找到对应的瓦片，进行后续操作
                if (!tile_json.contains("image")) {   // 没有image字段的话不符合数据格式要求，直接返回空的瓦片信息
                    spdlog::error("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。", tileset_it->first, tile_id);
                    return default_tile_info;
                }
                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto texture_id = resolve_path(tile_json["image"].get<std::string>(), file_path);
                // 先确认图片尺寸
                auto image_width = tile_json.value("imagewidth", 0);
                auto image_height = tile_json.value("imageheight", 0);
                // 从json中获取源矩形信息
                sf::IntRect texture_rect = {      // tiled中源矩形信息只有设置了才会有值，没有就是默认值
                    {tile_json.value("x", 0),
                    tile_json.value("y", 0)},
                    {tile_json.value("width", image_width),    // 如果未设置，则使用图片尺寸
                    tile_json.value("height", image_height)}
                };
                sf::Sprite sprite{*context_.get_resource_manager().get_texture(texture_id), texture_rect};
                auto tile_type = get_tile_type(tile_json);      // 有了瓦片json，直接获取瓦片类型
                return engine::component::TileInfo(sprite, tile_type);
            }
        }
    }
    // 如果能走到这里，说明查找失败，返回空的瓦片信息
    spdlog::error("图块集 '{}' 中未找到gid为 {} 的瓦片。", tileset_it->first, gid);
    return default_tile_info;
}

void LevelLoader::load_tileset(std::string_view tileset_path, int first_gid) {
    auto path = std::filesystem::path(tileset_path);
    std::ifstream tileset_file(path);
    if (!tileset_file.is_open()) {
        spdlog::error("无法打开 Tileset 文件: {}", tileset_path);
        return;
    }

    nlohmann::json ts_json;
    try {
        tileset_file >> ts_json;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("解析 Tileset json 文件 ‘{}’ 失败：{} (at byte {})", tileset_path, e.what(), e.byte);
        return;
    }
    ts_json["file_path"] = tileset_path;    // 将文件路径储存到json中，后续解析图片路径时需要
    tileset_data_[first_gid] = std::move(ts_json);
    spdlog::info("Tileset 文件 ‘{}’ 加载完成，firstgid：{}", tileset_path, first_gid);
}

std::string LevelLoader::resolve_path(std::string_view relative_path, std::string_view file_path) {
    try {
    // 获取地图文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
    auto map_dir = std::filesystem::path(file_path).parent_path();
    // 合并路径（相对于可执行文件）并返回。 /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，
                                      /*  得到一个干净的路径 */
    auto final_path = std::filesystem::canonical(map_dir / relative_path);
    return final_path.string();
    } catch (const std::exception& e) {
        spdlog::error("解析路径失败: {}", e.what());
        return std::string(relative_path);
    }
}

} // namespace engine::scene