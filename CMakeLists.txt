cmake_minimum_required(VERSION 3.22.1)

# set(WIN32MSVC ON)
# set(ANDROIDNATIVE ON)
set(ANDROIDSTUDIO ON)

# set(BUILD_EXECUTABLE ON)
set(BUILD_SHARED ON)
# set(BUILD_STATIC ON)

if (WIN32MSVC)
    project(TTinyRenderer)

    set(CMAKE_CXX_STANDARD 17)

    set(SOURCES
            ${CMAKE_SOURCE_DIR}/main.cpp
            ${CMAKE_SOURCE_DIR}/rasterizer.cpp
            ${CMAKE_SOURCE_DIR}/texture.cpp
            ${CMAKE_SOURCE_DIR}/shader.cpp
            ${CMAKE_SOURCE_DIR}/aarch64_neon.s
    )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

    if (BUILD_EXECUTABLE)
        add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
    elseif (BUILD_SHADED)
         add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
    else (BUILD_STATIC)
        add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})
    endif ()

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

elseif (ANDROIDNATIVE)
    if (WIN32)
        set(CMAKE_SYSTEM_NAME Android)
        set(ANDROID_PLATFORM 27)
        set(ANDROID_ABI arm64-v8a)
        set(ANDROID_NDK "C:/Users/DDD/AppData/Local/Android/Sdk/ndk/android-ndk-r27c")
        set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")
    endif()

    project(TTinyRenderer)

    enable_language(ASM)

    set(CMAKE_CXX_STANDARD 17)

    if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        add_compile_options(
                -O3                           # 启用最高级别优化
                -march=armv8-a+simd           # 启用 NEON 指令集
                -finline-functions            # 启用函数内联
                -mllvm -inline-threshold=1000 # 提高 Clang 内联可能性
        )
    endif()

    set(SOURCES
            ${CMAKE_SOURCE_DIR}/main.cpp
            ${CMAKE_SOURCE_DIR}/rasterizer.cpp
            ${CMAKE_SOURCE_DIR}/texture.cpp
            ${CMAKE_SOURCE_DIR}/shader.cpp
            ${CMAKE_SOURCE_DIR}/aarch64_neon.s
    )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

    if (BUILD_EXECUTABLE)
        add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
    elseif (BUILD_SHADED)
        add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
    else (BUILD_STATIC)
        add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})
    endif ()

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

    target_link_libraries(${CMAKE_PROJECT_NAME} android log)

else (ANDROIDSTUDIO)
    project("tinyrenderer")

    set(CMAKE_CXX_STANDARD 17)

    enable_language(ASM)

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        add_compile_options(
                -O3                           # 启用最高级别优化
                -march=armv8-a+simd           # 启用 NEON 指令集
                -finline-functions            # 启用函数内联
                -mllvm -inline-threshold=1000 # 提高 Clang 内联可能性
        )
    endif ()

    set(SOURCES
            ${CMAKE_SOURCE_DIR}/tinyrenderer.cpp
            ${CMAKE_SOURCE_DIR}/rasterizer.cpp
            ${CMAKE_SOURCE_DIR}/texture.cpp
            ${CMAKE_SOURCE_DIR}/shader.cpp
            ${CMAKE_SOURCE_DIR}/aarch64_neon.s
    )

    add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})

    target_link_libraries(${CMAKE_PROJECT_NAME} android log)
endif ()
