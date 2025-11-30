#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <SFML/Audio.hpp>

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::audio {
/**
 * @brief 基于 SFML 的音频播放器，完全兼容你当前的 ResourceManager
 * 音量范围：0-100（SFML 原生，直观）
 */
class AudioPlayer final {
public:
    explicit AudioPlayer(engine::resource::ResourceManager* rm);
    ~AudioPlayer();

    // 删除拷贝和移动（单例风格）
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    AudioPlayer(AudioPlayer&&) = delete;
    AudioPlayer& operator=(AudioPlayer&&) = delete;

    // --- 音效控制 ---
    /**
     * @brief 播放音效
     * @param sound_path 音效路径
     * @param position 音效位置（不要使用此参数，空间音频目前不完善）
     * @param volume 音量 0-100，默认 100
     * @param loop 是否循环，默认 false
     * @return 返回 sf::Sound* 可用于后续控制（pause/stop/setVolume），失败返回 nullptr
     */
    sf::Sound* play_sound(std::string_view sound_path, sf::Vector2f position = {0.f, 0.f}, float volume = 100.f, bool loop = false);

    /**
     * @brief 设置所有音效的全局音量
     */
    void set_sound_volume(float volume);  // 0-100

    /**
     * @brief 获取当前音效音量（取第一个活跃音效为准）
     */
    float get_sound_volume() const;

    // --- 音乐控制 ---
    /**
     * @brief 播放背景音乐（自动停止上一首）
     * @param music_path 音乐路径
     * @param loop 是否循环，默认 true
     * @return 成功返回 true
     */
    bool play_music(std::string_view music_path, bool loop = true);

    void stop_music();
    void pause_music();
    void resume_music();

    void set_music_volume(float volume);  // 0-100
    float get_music_volume() const;

private:
    engine::resource::ResourceManager* resource_manager_;

    // 正在播放的音效实例（用于控制音量、暂停等）
    std::vector<std::unique_ptr<sf::Sound>> active_sounds_;

    // 当前背景音乐路径（防止重复播放）
    std::string current_music_path_;
};
} // namespace engine::audio