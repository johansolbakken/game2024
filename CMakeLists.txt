cmake_minimum_required(VERSION 3.5)

project(JohanGame2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(JohanGame2024 
    MACOSX_BUNDLE
    src/main.cpp
    src/core/application.cpp
    src/platform/macoswindow.mm
)
target_include_directories(JohanGame2024 PRIVATE src)
target_link_libraries(JohanGame2024 PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo")