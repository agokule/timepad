cmake_minimum_required(VERSION 3.20)

project(Timepad LANGUAGES CXX)

file(GLOB_RECURSE CLIENT_SOURCES "src/*.cpp" "src/*.h")

add_executable(Timepad ${CLIENT_SOURCES}
    ./dependencies/imgui/imgui.cpp
    ./dependencies/imgui/imgui_draw.cpp
    ./dependencies/imgui/imgui_tables.cpp
    ./dependencies/imgui/imgui_widgets.cpp
    ./dependencies/imgui/imgui_demo.cpp
    ./dependencies/imgui/backends/imgui_impl_sdl3.cpp
    ./dependencies/imgui/backends/imgui_impl_sdlrenderer3.cpp
    ./dependencies/imgui/misc/cpp/imgui_stdlib.cpp
)
target_compile_definitions(Timepad PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Include directories
target_include_directories(Timepad PRIVATE 
    ./src
    ./dependencies/imgui
    ./dependencies/imgui/misc/cpp
    ./dependencies/imgui/backends/
)

# Link libraries
target_link_libraries(Timepad PRIVATE
    SDL3::SDL3
)

