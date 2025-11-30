#include "context.hpp"
#include "input_manager.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "resource_manager.hpp"
#include "physics_engine.hpp"
#include "audio_player.hpp"
#include <spdlog/spdlog.h>

namespace engine::core {
Context::Context(engine::input::InputManager& input_manager
               , engine::render::Renderer& renderer
               , engine::render::Camera& camera
               , engine::resource::ResourceManager& resource_manager
               , engine::physics::PhysicsEngine& physics_engine
               , engine::audio::AudioPlayer& audio_player)
    : input_manager_{input_manager}
    , renderer_{renderer}
    , camera_{camera}
    , resource_manager_{resource_manager}
    , physics_engine_{physics_engine}
    , audio_player_{audio_player} {
    spdlog::trace("上下文已创建并初始化");
}
} // namespace engine::core