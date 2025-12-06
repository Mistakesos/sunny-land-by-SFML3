# SunnyLand

**SunnyLand** is a cross-platform 2D platformer and a complete game engine example developed in modern C++ using **SFML 3**, **nlohmann/json**, **spdlog**, and **Tiled** map editor.

Fully **CC0 1.0** – you can use the code and assets for any purpose, including commercial projects.

## Control
```
A,D / left,right - to move;
W,S / up,down - to climb;
J / Space - to jump;
P / ESC - show menu and pause;
```

## ScreenShot
<img src="https://theorhythm.top/gamedev/SL/SL_screenshot_1.png" style='width: 600px;'/>
<img src="https://theorhythm.top/gamedev/SL/SL_screenshot_2.png" style='width: 600px;'/>
<img src="https://theorhythm.top/gamedev/SL/SL_screenshot_3.png" style='width: 600px;'/>
<img src="https://theorhythm.top/gamedev/SL/SL_screenshot_4.png" style='width: 600px;'/>

## Third-party libraries
* [SFML3](https://github.com/SFML/SFML)
* [nlohmann-json](https://github.com/nlohmann/json)
* [spdlog](https://github.com/gabime/spdlog)

## How to build (if you on windows, you need modify CmakeLists.txt to find library correctly)
```bash
git clone https://github.com/Mistakesos/sunny-land-by-SFML3.git
cd sunny-land-by-SFML3
cmake -S . -B build
cmake --build build
```

## Features
- Full Tiled map loader (objects, custom properties, embedded animation/sound JSON)
- Player + Enemy + UI state machine architecture
- Data-driven animation, sound, and input configuration
- Industrial-grade UI system with event bubbling and state machine buttons
- Coyote time, variable jump height, ladder climbing, one-way platforms, etc.
  
# Credits
- sprite
    - https://ansimuz.itch.io/sunny-land-pixel-game-art
- FX
    - https://ansimuz.itch.io/sunny-land-pixel-game-art
- font
    - https://timothyqiu.itch.io/vonwaon-bitmap
- UI
    - https://markiro.itch.io/hud-asset-pack
    - https://bdragon1727.itch.io/platformer-ui-buttons
- sound
    - https://taira-komori.jpn.org/
    - https://pixabay.com/sound-effects/dead-8bit-41400/
    - https://pixabay.com/sound-effects/cartoon-jump-6462/
    - https://pixabay.com/zh/sound-effects/frog-quak-81741/
    - https://mmvpm.itch.io/platformer-sound-fx-pack
    - https://kasse.itch.io/ui-buttons-sound-effects-pack
- music
    - https://ansimuz.itch.io/sunny-land-pixel-game-art
