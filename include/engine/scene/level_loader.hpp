#pragma once
#include "context.hpp"
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace engine::scene {
class Scene;

class LevelLoader final {
public:
    explicit LevelLoader(engine::core::Context& context);
    ~LevelLoader();

    /**
     * @brief 加载关卡数据到指定的 Scene 对象中。
     * @param map_path Tiled JSON 地图文件的路径。
     * @param scene 要加载数据的目标 Scene 对象。
     * @return bool 是否加载成功。
     */
    [[nodiscard]]bool load_level(std::string_view level_path, Scene& scene);

private:
    void load_image_layer(const nlohmann::json& layer_json, Scene& scene);  ///< @brief 加载图片层
    void load_tile_layer(const nlohmann::json& layer_json, Scene& scene);   ///< @brief 加载瓦片图层
    void load_object_layer(const nlohmann::json& layer_json, Scene& scene); ///< @brief 加载对象图层

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如：
     * 1. 文件路径："assets/maps/level1.tmj"
     * 2. 相对路径："../textures/Layers/back.png"
     * 3. 最终路径："assets/textures/Layers/back.png"
     * @param relative_path 相对路径（相对于文件）
     * @param file_path 文件路径
     * @return std::string 解析后的完整路径。
     */
    std::string resolve_path(std::string_view image_path);

    std::string map_path_;              ///< @brief 地图路径（拼接路径时需要）
    engine::core::Context& context_;    ///< @brief 上下文引用，用于加载资源
};
} // namespace engine::scene