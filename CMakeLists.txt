cmake_minimum_required(VERSION 3.22.1)

# 编译选项：BUILD_EXECUTABLE用于可执行文件编译，BUILD_ANDROID_LIB用于Android动态库编译
option(BUILD_EXECUTABLE "Build executable for Win/Mac/Linux/Android" ON)
option(BUILD_ANDROID_LIB "Build shared library for Android App" OFF)

if(BUILD_EXECUTABLE)
    # 可执行文件编译模式 (Win/Mac/Linux/Android)
    project(TTinyRenderer)

    # Set C++20 standard
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    enable_language(ASM)

    add_compile_options(-w)

    # Build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()

    # Optimization flags for smaller binary size
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-fvisibility=hidden -ffunction-sections -fdata-sections)
        # Platform-specific linker flags
        if(APPLE)
            add_link_options(-Wl,-dead_strip)
        else()
            add_link_options(-Wl,--gc-sections -Wl,--strip-all)
        endif()
    endif()

    file(GLOB_RECURSE COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/object/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rasterizer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shader/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp
    )

    file(GLOB SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${COMMON_SOURCES}
        # ${CMAKE_CURRENT_SOURCE_DIR}/aarch64_neon.s
    )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/ext
    )

    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

    # 定义资源目录
    set(RESOURCE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(RESOURCE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output)

    # 复制资源文件到输出目录
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${RESOURCE_SOURCE_DIR}/res
            ${RESOURCE_OUTPUT_DIR}/res
        COMMENT "Copying resource files to output directory"
    )

elseif(BUILD_ANDROID_LIB)
    # Android动态库编译模式
    project("tinyrenderer")

    # Set C++20 standard
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    enable_language(ASM)

    add_compile_options(-w)

    # Build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()

    # Optimization flags for smaller binary size
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-fvisibility=hidden -ffunction-sections -fdata-sections)
        add_link_options(-Wl,--gc-sections -Wl,--strip-all)
    endif()

    file(GLOB SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tinyrenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rasterizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/aarch64_neon.s
    )

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})

    target_compile_definitions(${PROJECT_NAME} PRIVATE kANDROID_LOG)

    target_link_libraries(${CMAKE_PROJECT_NAME} android log)
endif()
