cmake_minimum_required(VERSION 3.20)
include(FetchContent)

set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wall -O2")

project(Timepad LANGUAGES CXX)

# Fetch Version 3.4.10 of SDL
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG 8e37db5e797b6167f3a00d697d816a684bd259c7
    FIND_PACKAGE_ARGS # if SDL happens to be installed on the system, use that
)
FetchContent_MakeAvailable(SDL3)

# Fetch Version 1.92.8 of Dear ImGui
FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 8936b58fe26e8c3da834b8f60b06511d537b4c63
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(ImGui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CLIENT_SOURCES "src/*.cpp" "src/*.h")

if (GCC)
    add_compile_options("$<$<CONFIG:Debug>:-g3;-O0>")
endif()

add_executable(Timepad ${CLIENT_SOURCES}
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ./dependencies/miniaudio/miniaudio.cpp
)
target_compile_definitions(Timepad PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Include directories
target_include_directories(Timepad PRIVATE
    ./src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
    ${imgui_SOURCE_DIR}/backends/
    ./dependencies/IconFontCppHeaders/
    ./dependencies/miniaudio/
)

option(DISTRIBUTION_MODE "Enable distribution mode" OFF)

# Configure the preprocessor definitions based on build type
if(DISTRIBUTION_MODE)
    target_compile_definitions(Timepad PUBLIC DISTRIBUTION)
endif()

# Link libraries
target_link_libraries(Timepad PRIVATE
    SDL3::SDL3
)

