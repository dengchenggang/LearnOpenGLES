# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name for this subdirectory.
project("module")

# Include directories for this module
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Collect source files from subdirectories
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/common COMMON_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/nvs NVS_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} MODULE_ROOT_SRC_LIST)

# Combine all source lists
set(MODULE_ALL_SRC_LIST
    ${COMMON_SRC_LIST}
    ${NVS_SRC_LIST}
    ${MODULE_ROOT_SRC_LIST})

# 将源文件列表传递给父级 CMakeLists.txt
set(MODULE_ALL_SRC_LIST
    ${MODULE_ALL_SRC_LIST}
    PARENT_SCOPE)

# Export include directories for other modules to use
set(MODULE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE)

# Enable C++17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)