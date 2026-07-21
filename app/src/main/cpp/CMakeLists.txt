# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("learnopengles")
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.

## thirdparty (GLM)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

## framework
include_directories(${PROJECT_SOURCE_DIR}/utils)
include_directories(${PROJECT_SOURCE_DIR}/utils/config)
include_directories(${PROJECT_SOURCE_DIR}/framework/log)
include_directories(${PROJECT_SOURCE_DIR}/framework/render)
include_directories(${PROJECT_SOURCE_DIR}/framework/filesystem)
include_directories(${PROJECT_SOURCE_DIR}/framework/filesystem/reader)
include_directories(${PROJECT_SOURCE_DIR}/framework/filesystem/writer)
include_directories(${PROJECT_SOURCE_DIR}/framework/video)
include_directories(${PROJECT_SOURCE_DIR}/framework/video/common)
include_directories(${PROJECT_SOURCE_DIR}/framework/video/camera)
include_directories(${PROJECT_SOURCE_DIR}/framework/video/image)
include_directories(${PROJECT_SOURCE_DIR}/framework)

aux_source_directory(${PROJECT_SOURCE_DIR}/utils       FRAMEWORK_UTILS_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/utils/config UTILS_CONFIG_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/render FRAMEWORK_RENDER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/filesystem FRAMEWORK_FILESYSTEM_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/filesystem/reader FRAMEWORK_FILESYSTEM_READER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/filesystem/writer FRAMEWORK_FILESYSTEM_WRITER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/video       FRAMEWORK_VIDEO_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/video/common FRAMEWORK_VIDEO_COMMON_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/video/camera FRAMEWORK_VIDEO_CAMERA_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework/video/image FRAMEWORK_VIDEO_IMAGE_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/framework        FRAMEWORK_SRC_LIST)

set(FRAMEWORK_ALL_SRC_LIST
    ${FRAMEWORK_UTILS_SRC_LIST}
    ${UTILS_CONFIG_SRC_LIST}
    ${FRAMEWORK_RENDER_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_READER_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_WRITER_SRC_LIST}
    ${FRAMEWORK_VIDEO_SRC_LIST}
    ${FRAMEWORK_VIDEO_COMMON_SRC_LIST}
    ${FRAMEWORK_VIDEO_CAMERA_SRC_LIST}
    ${FRAMEWORK_VIDEO_IMAGE_SRC_LIST}
    ${FRAMEWORK_SRC_LIST})

## engine
include_directories(${PROJECT_SOURCE_DIR}/engine/common)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/mesh)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/material)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/camera)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/transform)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer)
include_directories(${PROJECT_SOURCE_DIR}/engine)

aux_source_directory(${PROJECT_SOURCE_DIR}/engine/common    ENGINE_COMMON_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/mesh ENGINE_RENDERER_MESH_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/material ENGINE_RENDERER_MATERIAL_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/camera ENGINE_RENDERER_CAMERA_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/transform ENGINE_RENDERER_TRANSFORM_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer  ENGINE_RENDERER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine           ENGINE_SRC_LIST)

set(ENGINE_ALL_SRC_LIST
    ${ENGINE_COMMON_SRC_LIST}
    ${ENGINE_RENDERER_MESH_SRC_LIST}
    ${ENGINE_RENDERER_MATERIAL_SRC_LIST}
    ${ENGINE_RENDERER_CAMERA_SRC_LIST}
    ${ENGINE_RENDERER_TRANSFORM_SRC_LIST}
    ${ENGINE_RENDERER_SRC_LIST}
    ${ENGINE_SRC_LIST})

## platform
include_directories(${PROJECT_SOURCE_DIR}/platform)
include_directories(${PROJECT_SOURCE_DIR}/platform/egl)

aux_source_directory(${PROJECT_SOURCE_DIR}/platform/egl    PLATFORM_EGL_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/platform        PLATFORM_SRC_LIST)

set(PLATFORM_ALL_SRC_LIST
    ${PLATFORM_EGL_SRC_LIST}
    ${PLATFORM_SRC_LIST})

## 编译动态库
add_library(${CMAKE_PROJECT_NAME} SHARED
        ${FRAMEWORK_ALL_SRC_LIST}
        ${ENGINE_ALL_SRC_LIST}
        ${PLATFORM_ALL_SRC_LIST})

find_library(egl-lib EGL)
find_library(gles-lib GLESv3)
find_library(mediandk-lib mediandk)
find_library(camera2ndk-lib camera2ndk)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        mediandk
        camera2ndk
        ${egl-lib}
        ${gles-lib})

# 启用 C++17 支持
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)