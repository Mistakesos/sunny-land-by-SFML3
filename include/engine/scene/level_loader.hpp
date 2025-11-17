#pragma once
#include "context.hpp"
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json_fwd.hpp>
#include <map>
#include <string>

namespace engine::component {
    struct TileInfo;
    enum class TileType;
} // namespace engine::component

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
     * @brief 根据瓦片json对象获取瓦片类型
     * @param tile_json 瓦片json数据
     * @return 瓦片类型
     */
    engine::component::TileType get_tile_type(const nlohmann::json& tile_json);

    /**
     * @brief 根据（单一图片）图块集中的id获取瓦片类型
     * @param tileset_json 图块集json数据
     * @param local_id 图块集中的id
     * @return 瓦片类型
     */
    engine::component::TileType get_tile_type_by_id(const nlohmann::json& tileset_json, int local_id);
    
    /**
     * @brief 根据全局 ID 获取瓦片信息。
     * @param gid 全局 ID。
     * @return engine::component::TileInfo 瓦片信息。
     */
    engine::component::TileInfo get_tile_info_by_gid(int gid);
    
    /**
     * @brief 加载 Tiled tileset 文件 (.tsj)。
     * @param tileset_path Tileset 文件路径。
     * @param first_gid 此 tileset 的第一个全局 ID。
     */
    void load_tileset(std::string_view tileset_path, int first_gid);

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如：
     * 1. 文件路径："assets/maps/level1.tmj"
     * 2. 相对路径："../textures/Layers/back.png"
     * 3. 最终路径："assets/textures/Layers/back.png"
     * @param relative_path 相对路径（相对于文件）
     * @param file_path 文件路径
     * @return std::string 解析后的完整路径。
     */
    std::string resolve_path(std::string_view relative_path, std::string_view file_path);

    std::string map_path_;                          ///< @brief 地图路径（拼接路径时需要）
    sf::Vector2i map_size_;                         ///< @brief 地图尺寸（瓦片数量）
    sf::Vector2i tile_size_;                        ///< @brief 瓦片尺寸（像素）
    std::map<int, nlohmann::json> tileset_data_;    ///< @brief firstgid -> 瓦片集数据
    engine::core::Context& context_;                ///< @brief 上下文引用，用于加载资源
};
} // namespace engine::scene