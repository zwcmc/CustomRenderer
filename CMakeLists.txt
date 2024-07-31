cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME CustomRenderer)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/loader/AssetsLoader.cpp
    ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c
    src/base/Shader.cpp
    src/base/Texture.cpp
    src/base/Material.cpp
    src/renderer/Renderer.cpp
    src/renderer/Camera.cpp
    src/renderer/MeshRenderer.cpp
    src/renderer/glTFRenderer.cpp
    src/model/Mesh.cpp
    src/model/Quad.cpp
    src/model/Cube.cpp
)

set(HEADERS
    src/ptr.h
    src/loader/AssetsLoader.h
    src/base/Shader.h
    src/base/Texture.h
    src/base/Material.h
    src/renderer/Renderer.h
    src/renderer/Camera.h
    src/renderer/MeshRenderer.h
    src/renderer/glTFRenderer.h
    src/model/Mesh.h
    src/model/Quad.h
    src/model/Cube.h
    src/model/Models.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
endif()

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# glm
add_subdirectory(third_party/glm)
target_link_libraries(${PROJECT_NAME} glm)

# tinygltf
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/tinygltf)
target_link_libraries(${PROJECT_NAME} tinygltf)

# # imgui
# add_subdirectory(third_party/imgui)
# target_link_libraries(${PROJECT_NAME} imgui)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/glad/include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)