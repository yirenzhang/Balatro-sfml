cmake_minimum_required(VERSION 3.20)

project(Balatro-Cpp VERSION 1.0.0 LANGUAGES CXX)

# 1. 启用编译命令导出 (让 IntelliSense 自动工作，不再需要 c_cpp_properties.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# =========================================================
# SFML 配置
# =========================================================
set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/sfml/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system audio main REQUIRED) 

set(JSON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/nlohmann_json/include")

# =========================================================
# 源文件
# =========================================================
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.hpp")
# source_group 主要是为了 VS 里的文件夹视图，在 VS Code 里没啥用，但留着无妨
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${PROJECT_SOURCES})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# =========================================================
# 核心修改点：链接顺序 (GCC 非常严格！)
# =========================================================
# 顺序必须是：[调用者] 在 [被调用者] 之前
# Graphics 依赖 Window 和 System -> Graphics 放最前
# Window 依赖 System -> Window 放中间
# Audio 依赖 System -> Audio 放中间
# System 被大家依赖 -> System 放最后
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system  # <--- System 必须移到这里！
    sfml-main    # 处理 Windows 入口点
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${JSON_INCLUDE_DIR}"
)

# =========================================================
# 编译选项 & 窗口控制
# =========================================================
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
    # MinGW/GCC 走这里
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# 控制台窗口逻辑：
# Debug 模式 -> WIN32_EXECUTABLE=OFF (带黑框，方便看 cout)
# Release 模式 -> WIN32_EXECUTABLE=ON (无黑框，正经游戏模式)
# 这行代码在 MinGW 下会自动添加 -mwindows 标志
set_target_properties(${PROJECT_NAME} PROPERTIES 
    WIN32_EXECUTABLE "$<CONFIG:Release>"
)

# =========================================================
# 自动复制 DLL 和资源
# =========================================================
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/external/sfml/bin"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    )
endif()