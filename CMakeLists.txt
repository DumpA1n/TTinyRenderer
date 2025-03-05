cmake_minimum_required(VERSION 3.10)

project(TTinyRenderer)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

# include_directories(${CMAKE_SOURCE_DIR})

add_executable(TTinyRenderer main.cpp rasterizer.cpp texture.cpp shader.cpp)
