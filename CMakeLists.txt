cmake_minimum_required(VERSION 3.16)

project (rpg)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable("${CMAKE_PROJECT_NAME}")
target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")

add_subdirectory("thirdparty/raylib")
add_subdirectory("thirdparty/imgui")
add_subdirectory("thirdparty/rlImGui")

file(GLOB_RECURSE IMGUIFILEDIALOG_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGuiFileDialog/ImGuiFileDialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGuiFileDialog/ImGuiFileDialog.h"
)
target_sources(${PROJECT_NAME} PRIVATE ${IMGUIFILEDIALOG_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGuiFileDialog"
)

target_link_libraries(${PROJECT_NAME} PUBLIC raylib imgui rlImGui)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()
