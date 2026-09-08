set(ENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/level
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/mesh
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/material
    ${CMAKE_CURRENT_SOURCE_DIR}/component/base/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/component/base/transform
    ${CMAKE_CURRENT_SOURCE_DIR}/component/common
    ${CMAKE_CURRENT_SOURCE_DIR}/component/image
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/render
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/render/context
    ${CMAKE_CURRENT_SOURCE_DIR}/component
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    PARENT_SCOPE)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/common             ENGINE_COMMON_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/level              ENGINE_LEVEL_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/renderer/mesh      ENGINE_RENDERER_MESH_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/renderer/material  ENGINE_RENDERER_MATERIAL_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/base/camera    ENGINE_COMPONENT_CAMERA_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/base/transform ENGINE_COMPONENT_TRANSFORM_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/common    ENGINE_COMPONENT_COMMON_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/image      ENGINE_COMPONENT_IMAGE_SRC_LIST)
# ensure reconfigure picks up new files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/renderer           ENGINE_RENDERER_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/renderer/render    ENGINE_RENDERER_RENDER_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/renderer/render/context ENGINE_RENDERER_RENDER_CONTEXT_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component          ENGINE_COMPONENT_SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}                    ENGINE_SRC_LIST)

set(ENGINE_ALL_SRC_LIST
    ${ENGINE_COMMON_SRC_LIST}
    ${ENGINE_LEVEL_SRC_LIST}
    ${ENGINE_RENDERER_MESH_SRC_LIST}
    ${ENGINE_RENDERER_MATERIAL_SRC_LIST}
    ${ENGINE_COMPONENT_CAMERA_SRC_LIST}
    ${ENGINE_COMPONENT_TRANSFORM_SRC_LIST}
    ${ENGINE_COMPONENT_COMMON_SRC_LIST}
    ${ENGINE_COMPONENT_IMAGE_SRC_LIST}
    ${ENGINE_RENDERER_SRC_LIST}
    ${ENGINE_RENDERER_RENDER_SRC_LIST}
    ${ENGINE_RENDERER_RENDER_CONTEXT_SRC_LIST}
    ${ENGINE_COMPONENT_SRC_LIST}
    ${ENGINE_SRC_LIST}
    PARENT_SCOPE)
