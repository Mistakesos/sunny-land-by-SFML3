#include "audio_player.hpp"
#include "resource_manager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::audio {

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* rm)
    : resource_manager_(rm) {
    if (!resource_manager_) {
        throw std::runtime_error("AudioPlayer: ResourceManager 指针为空");
    }
    spdlog::info("AudioPlayer 初始化成功");
}

AudioPlayer::~AudioPlayer() {
    stop_music();
    active_sounds_.clear();
}

// ========================= 音效 =========================
sf::Sound* AudioPlayer::play_sound(std::string_view sound_path, sf::Vector2f, float volume, bool loop) {
    // 清理播放结束的音效
    std::erase_if(active_sounds_, [](const auto& s) {
        return s->getStatus() == sf::SoundSource::Status::Stopped;
    });

    auto* buffer = resource_manager_->get_sound(sound_path);
    if (!buffer) {
        spdlog::error("AudioPlayer: 无法加载音效 '{}'", sound_path);
        return nullptr;
    }

    auto sound = std::make_unique<sf::Sound>(*buffer);
    sound->setVolume(std::clamp(volume, 0.f, 100.f));
    sound->setLooping(loop);
    sound->play();

    active_sounds_.push_back(std::move(sound));

    spdlog::trace("AudioPlayer: 播放音效 '{}', 音量: {:.1f}, 循环: {}", sound_path, volume, loop);
    return active_sounds_.back().get();
}

void AudioPlayer::set_sound_volume(float volume) {
    volume = std::clamp(volume, 0.f, 100.f);
    for (auto& sound : active_sounds_) {
        sound->setVolume(volume);
    }
    spdlog::trace("AudioPlayer: 全局音效音量设为 {:.1f}", volume);
}

float AudioPlayer::get_sound_volume() const {
    if (active_sounds_.empty()) return 100.f;
    return active_sounds_.front()->getVolume();
}

// ========================= 音乐 =========================
bool AudioPlayer::play_music(std::string_view music_path, bool loop) {
    // 防止重复播放同一首
    if (current_music_path_ == music_path) {
        auto* music = resource_manager_->get_music(music_path);
        if (music && music->getStatus() == sf::Music::Status::Playing) {
            return true;
        }
    }

    // 先停止旧的（如果有）
    stop_music();

    auto* music = resource_manager_->get_music(music_path);
    if (!music) {
        spdlog::error("AudioPlayer: 无法加载音乐 '{}'", music_path);
        return false;
    }

    music->setLooping(loop);
    music->play();
    current_music_path_ = music_path;

    spdlog::info("AudioPlayer: 开始播放音乐 '{}', 循环: {}", music_path, loop);
    return true;
}

void AudioPlayer::stop_music() {
    if (current_music_path_.empty()) return;

    if (auto* music = resource_manager_->get_music(current_music_path_)) {
        music->stop();
    }
    current_music_path_.clear();
    spdlog::trace("AudioPlayer: 停止背景音乐");
}

void AudioPlayer::pause_music() {
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_->get_music(current_music_path_)) {
        music->pause();
    }
}

void AudioPlayer::resume_music() {
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_->get_music(current_music_path_)) {
        music->play();
    }
}

void AudioPlayer::set_music_volume(float volume) {
    volume = std::clamp(volume, 0.f, 100.f);
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_->get_music(current_music_path_)) {
        music->setVolume(volume);
    }
    spdlog::trace("AudioPlayer: 音乐音量设为 {:.1f}", volume);
}

float AudioPlayer::get_music_volume() const {
    if (current_music_path_.empty()) return 100.f;
    if (auto* music = resource_manager_->get_music(current_music_path_)) {
        return music->getVolume();
    }
    return 100.f;
}
} // namespace engine::audio