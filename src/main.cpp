#include "game.hpp"
#include <spdlog/spdlog.h>
int main() {
    spdlog::set_level(spdlog::level::off);
    
    engine::core::Game game;
    game.run();
}