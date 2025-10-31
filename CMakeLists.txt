# cmake配置

# 1、设置对cmake的最小版本
cmake_minimum_required(VERSION 3.29.1)

# 2、设置工程名
project(sunny_land)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

find_package(SFML REQUIRED COMPONENTS Audio Graphics)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# 6、设置目标对象（可执行文件）的输出目录。即app可执行文件就会输出到bin目录中
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# 4、获取指定目录下的所有cpp文件，用SRC_LIST变量代替
# 如果src目录下的cpp文件很多，在生成可执行文件时一个个写出来十分不便，所以可以使用这个命令方便使用
file(GLOB_RECURSE SRC_LIST CMAKE_CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/core/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/resource/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/render/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/input/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/component/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/object/*.cpp
)

# 5、生成可执行文件，这里的所有cpp文件用变量替代
add_executable(${PROJECT_NAME} ${SRC_LIST})

# 3、指定需要的头文件目录
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/engine/core
        ${PROJECT_SOURCE_DIR}/include/engine/resource
        ${PROJECT_SOURCE_DIR}/include/engine/render
        ${PROJECT_SOURCE_DIR}/include/engine/input
        ${PROJECT_SOURCE_DIR}/include/engine/component
        ${PROJECT_SOURCE_DIR}/include/engine/object
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        SFML::Audio 
        SFML::Graphics
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)
