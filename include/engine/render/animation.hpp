#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

namespace engine::render {
    struct AnimationFrame {
        sf::FloatRect source_rect;  // 纹理图集上此帧的区域
        sf::Time duration;          // 此帧持续时间
    };
    
    class Animation final {
    public:
        Animation(std::string_view name = "default", bool loop = true);
        ~Animation() = default;

        // 添加一帧
        void add_frame(const sf::FloatRect& source_rect, const sf::Time& duration);
        // 获取一帧
        // const AnimationFrame& get_frame(const sf::Time& time) const;
        // 获取全部帧
        const std::vector<AnimationFrame>& get_frames_vec() const;
        // 获取帧数量
        std::uint32_t get_frame_count() const;

        // 获取动画名称
        std::string_view get_name() const;
        // 设置动画名称
        void set_name(std::string_view name);
        // 获取动画持续时间
        const sf::Time& get_total_durationn() const;
        // 设置动画是否循环
        void set_looping(bool loop);
        // 判断动画是否循环
        bool is_looping() const;
        // 判断动画是否为空
        bool is_empty() const;
    private:
        std::string name_;                                  // 动画名称
        std::vector<AnimationFrame> frames_;                // 动画帧列表
        sf::Time total_duration_ = sf::Time::Zero;          // 动画的总持续时间
        bool is_loop_ = true;                               // 动画默认为重复
    };
} // namespace engine::render