cmake_minimum_required(VERSION 3.10)

project(TTinyRenderer)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    message(STATUS "Target Architecture: x86_64 (AMD64)")
    add_definitions(-DX86_64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64")
    message(STATUS "Target Architecture: AArch64 (ARM64)")
    add_definitions(-DAARCH64)
else()
    message(WARNING "Unknown Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# include_directories(${CMAKE_SOURCE_DIR})

# add_library(${CMAKE_PROJECT_NAME} SHARED main.cpp rasterizer.cpp texture.cpp shader.cpp)
add_executable(${CMAKE_PROJECT_NAME} main.cpp rasterizer.cpp texture.cpp shader.cpp)
