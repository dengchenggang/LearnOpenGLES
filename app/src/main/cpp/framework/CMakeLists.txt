include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/network/mongoose)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/network/mongoose   FRAMEWORK_NETWORK_MONGOOSE_SRC_LIST)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/utils       FRAMEWORK_UTILS_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/utils/config UTILS_CONFIG_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem FRAMEWORK_FILESYSTEM_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem/reader FRAMEWORK_FILESYSTEM_READER_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem/writer FRAMEWORK_FILESYSTEM_WRITER_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/video       FRAMEWORK_VIDEO_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/video/common FRAMEWORK_VIDEO_COMMON_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/video/camera FRAMEWORK_VIDEO_CAMERA_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/video/image FRAMEWORK_VIDEO_IMAGE_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}        FRAMEWORK_SRC_LIST)

set(FRAMEWORK_ALL_SRC_LIST
    ${FRAMEWORK_UTILS_SRC_LIST}
    ${UTILS_CONFIG_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_READER_SRC_LIST}
    ${FRAMEWORK_FILESYSTEM_WRITER_SRC_LIST}
    ${FRAMEWORK_VIDEO_SRC_LIST}
    ${FRAMEWORK_VIDEO_COMMON_SRC_LIST}
    ${FRAMEWORK_VIDEO_CAMERA_SRC_LIST}
    ${FRAMEWORK_VIDEO_IMAGE_SRC_LIST}
    ${FRAMEWORK_NETWORK_MONGOOSE_SRC_LIST}
    ${FRAMEWORK_SRC_LIST})

add_library(framework SHARED ${FRAMEWORK_ALL_SRC_LIST})

target_include_directories(framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(framework PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network/mongoose
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem/reader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem/writer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video/image
)

target_link_libraries(framework
    android
    log
    camera2ndk
    mediandk
)
