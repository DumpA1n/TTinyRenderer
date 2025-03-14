cmake_minimum_required(VERSION 3.10)

project(TTinyRenderer)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

# include_directories(${CMAKE_SOURCE_DIR})

# add_library(${CMAKE_PROJECT_NAME} SHARED main.cpp rasterizer.cpp texture.cpp shader.cpp)
add_executable(${CMAKE_PROJECT_NAME} main.cpp rasterizer.cpp texture.cpp shader.cpp)
