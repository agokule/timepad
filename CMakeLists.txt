cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(Timepad LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CLIENT_SOURCES "src/*.cpp" "src/*.h")

if (GCC)
    add_compile_options("$<$<CONFIG:Debug>:-g3;-O0>")
endif()

add_executable(Timepad ${CLIENT_SOURCES}
    ./dependencies/imgui/imgui.cpp
    ./dependencies/imgui/imgui_draw.cpp
    ./dependencies/imgui/imgui_tables.cpp
    ./dependencies/imgui/imgui_widgets.cpp
    ./dependencies/imgui/imgui_demo.cpp
    ./dependencies/imgui/backends/imgui_impl_sdl3.cpp
    ./dependencies/imgui/backends/imgui_impl_sdlrenderer3.cpp
    ./dependencies/imgui/misc/cpp/imgui_stdlib.cpp
    ./dependencies/miniaudio/miniaudio.cpp
)
target_compile_definitions(Timepad PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Include directories
target_include_directories(Timepad PRIVATE
    ./src
    ./dependencies/imgui
    ./dependencies/imgui/misc/cpp
    ./dependencies/imgui/backends/
    ./dependencies/SDL3/include/
    ./dependencies/IconFontCppHeaders/
    ./dependencies/miniaudio/
)

add_subdirectory(./dependencies/SDL3/ EXCLUDE_FROM_ALL)

# Link libraries
target_link_libraries(Timepad PRIVATE
    SDL3::SDL3
)

