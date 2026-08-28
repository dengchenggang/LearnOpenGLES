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
add_subdirectory(framework)

## engine
include_directories(${PROJECT_SOURCE_DIR}/engine/common)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/mesh)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/material)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/camera)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/transform)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer)
include_directories(${PROJECT_SOURCE_DIR}/engine/renderer/render)
include_directories(${PROJECT_SOURCE_DIR}/engine/actor)
include_directories(${PROJECT_SOURCE_DIR}/engine/component)
include_directories(${PROJECT_SOURCE_DIR}/engine)

aux_source_directory(${PROJECT_SOURCE_DIR}/engine/common    ENGINE_COMMON_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/mesh ENGINE_RENDERER_MESH_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/material ENGINE_RENDERER_MATERIAL_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/camera ENGINE_RENDERER_CAMERA_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/transform ENGINE_RENDERER_TRANSFORM_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer  ENGINE_RENDERER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/renderer/render ENGINE_RENDERER_RENDER_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/actor       ENGINE_ACTOR_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine/component   ENGINE_COMPONENT_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/engine           ENGINE_SRC_LIST)

set(ENGINE_ALL_SRC_LIST
    ${ENGINE_COMMON_SRC_LIST}
    ${ENGINE_RENDERER_MESH_SRC_LIST}
    ${ENGINE_RENDERER_MATERIAL_SRC_LIST}
    ${ENGINE_RENDERER_CAMERA_SRC_LIST}
    ${ENGINE_RENDERER_TRANSFORM_SRC_LIST}
    ${ENGINE_RENDERER_SRC_LIST}
    ${ENGINE_RENDERER_RENDER_SRC_LIST}
    ${ENGINE_ACTOR_SRC_LIST}
    ${ENGINE_COMPONENT_SRC_LIST}
    ${ENGINE_SRC_LIST})

## view
include_directories(${PROJECT_SOURCE_DIR}/view)

aux_source_directory(${PROJECT_SOURCE_DIR}/view            VIEW_SRC_LIST)

set(PLATFORM_ALL_SRC_LIST
    ${VIEW_SRC_LIST})

## 编译动态库
add_library(${CMAKE_PROJECT_NAME} SHARED
        ${PROJECT_SOURCE_DIR}/NativeBridge.cpp
        ${ENGINE_ALL_SRC_LIST}
        ${PLATFORM_ALL_SRC_LIST})

find_library(egl-lib EGL)
find_library(gles-lib GLESv3)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        ${egl-lib}
        ${gles-lib}
        framework)

# 启用 C++17 支持
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)