cmake_minimum_required(VERSION 3.20)

project(Balatro-Cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED) 

set(JSON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/nlohmann_json/include")

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.hpp")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${JSON_INCLUDE_DIR}"
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)