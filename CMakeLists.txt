cmake_minimum_required(VERSION 3.10)

# if (WIN32)
#     set(CMAKE_SYSTEM_NAME Android)
#     set(CMAKE_SYSTEM_VERSION 27)
#     set(ANDROID_PLATFORM 27)
#     set(ANDROID_ABI arm64-v8a)
#     set(ANDROID_NDK "C:/Users/DDD/AppData/Local/Android/Sdk/ndk/android-ndk-r27c")
#     set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")
# endif()

project(TTinyRenderer)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

# include_directories(${CMAKE_SOURCE_DIR})

# add_library(${CMAKE_PROJECT_NAME} SHARED main.cpp rasterizer.cpp texture.cpp shader.cpp)
add_executable(${CMAKE_PROJECT_NAME} main.cpp rasterizer.cpp texture.cpp shader.cpp)
