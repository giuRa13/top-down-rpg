cmake_minimum_required(VERSION 3.16)
project(rlImGui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB RLIMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(rlImGui STATIC ${RLIMGUI_SOURCES})

target_include_directories(rlImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../imgui
    ${CMAKE_SOURCE_DIR}/../raylib/src
)

target_link_libraries(rlImGui PUBLIC raylib)