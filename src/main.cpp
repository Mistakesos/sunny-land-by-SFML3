#include "game.hpp"
#include <spdlog/spdlog.h>
int main() {
    // spdlog::set_level(spdlog::level::debug);
    
    engine::core::Game game;
    game.run();
}