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

enable_language(ASM)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

# include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/rasterizer.cpp
    ${CMAKE_SOURCE_DIR}/texture.cpp
    ${CMAKE_SOURCE_DIR}/shader.cpp
    # ${CMAKE_SOURCE_DIR}/aarch64_neon.s
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
# add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
