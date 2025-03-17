cmake_minimum_required(VERSION 3.10)

if (WIN32 AND CMAKE_CXX_COMPILER MATCHES "android-ndk")
    set(JNI_CALL ON)
    set(CMAKE_SYSTEM_NAME Android)
    set(ANDROID_PLATFORM 27)
    set(ANDROID_ABI arm64-v8a)
    set(ANDROID_NDK "C:/Users/DDD/AppData/Local/Android/Sdk/ndk/android-ndk-r27c")
    set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")
endif()

project(TTinyRenderer)

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if (JNI_CALL AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    add_compile_options(
        -O3                           # 启用最高级别优化
        -march=armv8-a+simd           # 启用 NEON 指令集
        -finline-functions            # 启用函数内联
        -mllvm -inline-threshold=1000 # 提高 Clang 内联可能性
    )
endif()

enable_language(ASM)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)

if (JNI_CALL)
    set(SOURCES
        ${CMAKE_SOURCE_DIR}/tinyrenderer.cpp
        ${CMAKE_SOURCE_DIR}/rasterizer.cpp
        ${CMAKE_SOURCE_DIR}/texture.cpp
        ${CMAKE_SOURCE_DIR}/shader.cpp
        ${CMAKE_SOURCE_DIR}/aarch64_neon.s
    )
else()
    set(SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp
        ${CMAKE_SOURCE_DIR}/rasterizer.cpp
        ${CMAKE_SOURCE_DIR}/texture.cpp
        ${CMAKE_SOURCE_DIR}/shader.cpp
        ${CMAKE_SOURCE_DIR}/aarch64_neon.s
    )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

if (JNI_CALL)
    add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
else()
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
endif()
# add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${CMAKE_PROJECT_NAME} android log)
endif()
