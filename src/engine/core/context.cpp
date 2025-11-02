#include "context.hpp"
#include "input_manager.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "resource_manager.hpp"
#include <spdlog/spdlog.h>

namespace engine::core {
Context::Context(engine::input::InputManager& input_manager,
                               engine::render::Renderer& renderer,
                               engine::render::Camera& camera,
                               engine::resource::ResourceManager& resource_manager)
    : input_manager_{input_manager}
    , renderer_{renderer}
    , camera_{camera}
    , resource_manager_{resource_manager} {
    spdlog::trace("上下文已创建并初始化");
}

engine::input::InputManager& Context::get_input_manager() const {
    return input_manager_;
}

engine::render::Renderer& Context::get_renderer() const {
    return renderer_;
}

engine::render::Camera& Context::get_camera() const {
    return camera_;
}

engine::resource::ResourceManager& Context::get_resource_manager() const {
    return resource_manager_;
}
} // namespace engine::core