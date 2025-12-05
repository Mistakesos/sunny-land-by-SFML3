#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp> 

namespace game::data {
/**
 * @brief 管理不同游戏场景之间的游戏状态
 *
 * 存储玩家生命值、分数、当前关卡等信息，
 * 使这些数据在场景切换时能够保持。
 */
class SessionData final {
public:
    SessionData() = default;
    ~SessionData() = default;

    // 删除复制和移动操作以防止意外复制
    SessionData(const SessionData&) = delete;
    SessionData& operator=(const SessionData&) = delete;
    SessionData(SessionData&&) = delete;
    SessionData& operator=(SessionData&&) = delete;

    // --- Getters ---
    int get_current_health() const { return current_health_; }
    int get_max_health() const { return max_health_; }
    int get_current_score() const { return current_score_; }
    int get_high_score() const { return high_score_; }
    int get_level_health() const { return level_health_; }
    int get_level_score() const { return level_score_; }
    std::string_view get_map_path() const { return map_path_; }
    bool get_is_win() const { return is_win_; }

    // --- Setters ---
    void set_current_health(int health);
    void set_max_health(int max_health);
    void add_score(int score_to_add);
    void set_high_score(int high_score) { high_score_= high_score; }
    void set_level_health(int level_health) {level_health_ = level_health; }
    void set_level_score(int level_score) {level_score_ = level_score; }
    void set_map_path(std::string_view map_path) { map_path_ = map_path; }
    void set_is_win(bool is_win) { is_win_ = is_win; }

    void reset();                                           ///< @brief 重置游戏数据以准备开始新游戏（保留最高分）
    void set_next_level(std::string_view map_path);         ///< @brief 设置下一个场景信息（地图、关卡开始时的得分生命）
    bool save_to_file(std::string_view filename) const;     ///< @brief 将当前游戏数据保存到JSON文件（存档）
    bool load_from_file(std::string_view filename);         ///< @brief 从JSON文件中读取游戏数据（读档）
    bool sync_high_score(std::string_view filename);        ///< @brief 同步最高分(文件与当前分数取最大值)

private:
    int current_health_ = 3;
    int max_health_ = 3;
    int current_score_ = 0;
    int high_score_ = 0;
    bool is_win_ = false;
    
    int level_health_ = 3;          ///< @brief 进入关卡时的生命值（读/存档用）
    int level_score_ = 0;           ///< @brief 进入关卡时的得分（读/存档用）
    std::string map_path_ = "assets/maps/level_1.tmj";
};
} // namespace game::state