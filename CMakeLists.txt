cmake_minimum_required(VERSION 3.21)
project(bita LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Dawn CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Embed WGSL shaders as C++ string literals at configure time.
# Make CMake re-run configure when a shader changes so the generated
# header (and therefore the binary) stays in sync.
set_property(
    DIRECTORY APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.wgsl"
)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.wgsl" TRIANGLE_WGSL_SOURCE)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.wgsl.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/triangle.wgsl.h"
    @ONLY
)

add_executable(bita
    src/main.cpp
    src/app.cpp
    src/renderer.cpp
    src/ui/ui.cpp
    src/ui/rendered.cpp
    src/ui/components/square.cpp
)

target_include_directories(bita PRIVATE include ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(bita PRIVATE dawn::webgpu_dawn glfw)
