#include "level_loader.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"
#include "game_object.hpp"
#include "component.hpp"
#include "transform_component.hpp"
#include "parallax_component.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <filesystem>

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

    // 3、加载图层数据
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
    auto texture_id = resolve_path(image_path);

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
}

void LevelLoader::load_object_layer(const nlohmann::json& layer_json, Scene& scene) {
}

std::string LevelLoader::resolve_path(std::string_view image_path) {
    try {   
    // 获取地图文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
    auto map_dir = std::filesystem::path(map_path_).parent_path();
    // 合并路径（相对于可执行文件）并返回。 /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，
                                      /*  得到一个干净的路径 */
    auto final_path = std::filesystem::canonical(map_dir / image_path);
    return final_path.string();
    } catch (const std::exception& e) {
        spdlog::error("解析路径失败: {}", e.what());
        return std::string(image_path);
    }
}

} // namespace engine::scene