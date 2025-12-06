# 设置对cmake的最小版本
cmake_minimum_required(VERSION 3.29.1)

# 设置工程名
project(sunny_land)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# 添加编译器选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# 手动设置库位置，或者配置环境变量
if (WIN32)
    set(CMAKE_PREFIX_PATH "Your Libs path")
endif()

# 需要的库
find_package(SFML REQUIRED COMPONENTS Audio Graphics)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# 设置目标对象（可执行文件）的输出目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# 获取指定目录下的所有cpp文件，用SRC_LIST变量代替
# 如果src目录下的cpp文件很多，在生成可执行文件时一个个写出来十分不便，所以可以使用这个命令方便使用
file(GLOB_RECURSE SRC_LIST CMAKE_CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/core/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/resource/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/render/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/input/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/component/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/object/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/scene/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/physics/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/audio/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/ui/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/ui/state/*.cpp
    ${PROJECT_SOURCE_DIR}/src/game/scene/*.cpp
    ${PROJECT_SOURCE_DIR}/src/game/data/*.cpp
    ${PROJECT_SOURCE_DIR}/src/game/component/*.cpp
    ${PROJECT_SOURCE_DIR}/src/game/component/state/*.cpp
    ${PROJECT_SOURCE_DIR}/src/game/component/ai/*.cpp
)

# 统一生成可执行文件
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Windows 下隐藏黑窗（等价于 WIN32 宏）
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 指定需要的头文件目录
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/engine/utils
        ${PROJECT_SOURCE_DIR}/include/engine/core
        ${PROJECT_SOURCE_DIR}/include/engine/resource
        ${PROJECT_SOURCE_DIR}/include/engine/render
        ${PROJECT_SOURCE_DIR}/include/engine/input
        ${PROJECT_SOURCE_DIR}/include/engine/component
        ${PROJECT_SOURCE_DIR}/include/engine/object
        ${PROJECT_SOURCE_DIR}/include/engine/scene
        ${PROJECT_SOURCE_DIR}/include/engine/physics
        ${PROJECT_SOURCE_DIR}/include/engine/audio
        ${PROJECT_SOURCE_DIR}/include/engine/ui
        ${PROJECT_SOURCE_DIR}/include/engine/ui/state
        ${PROJECT_SOURCE_DIR}/include/game/scene
        ${PROJECT_SOURCE_DIR}/include/game/data
        ${PROJECT_SOURCE_DIR}/include/game/component
        ${PROJECT_SOURCE_DIR}/include/game/component/state
        ${PROJECT_SOURCE_DIR}/include/game/component/ai
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SFML::Audio
        SFML::Graphics
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)
