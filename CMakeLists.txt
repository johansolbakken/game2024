cmake_minimum_required(VERSION 3.5)

project(JohanGame2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor/glfw)

add_executable(JohanGame2024 
    MACOSX_BUNDLE
    src/main.cpp
    src/core/application.cpp
    src/renderer/renderer.cpp
    # src/platform/macoswindow.mm
    src/platform/glfwwindow.cpp
    src/platform/metalrenderer.mm
)
target_include_directories(JohanGame2024 PRIVATE src vendor/glm vendor/glfw/include)
target_link_libraries(JohanGame2024 PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework Metal" "-framework QuartzCore" "-framework MetalKit" glfw)