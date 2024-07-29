cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME CustomRenderer)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c
    src/opengl/Shader.cpp
    src/opengl/ShaderProgram.cpp
    src/opengl/VertexArray.cpp
    src/opengl/VertexBuffer.cpp
    src/opengl/ElementBuffer.cpp
    src/renderer/Renderer.cpp
    src/renderer/Camera.cpp
    src/tool/AssetsLoader.cpp
    src/model/Mesh.cpp
    src/model/Quad.cpp
    src/model/Cube.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

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

# # imgui
# add_subdirectory(third_party/imgui)
# target_link_libraries(${PROJECT_NAME} imgui)

# # tinygltf
# set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
# set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL "" FORCE)
# set(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF CACHE BOOL "" FORCE)
# add_subdirectory(third_party/tinygltf)
# target_link_libraries(${PROJECT_NAME} tinygltf)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/glfw/include)
# target_include_directories(${PROJECT_NAME} PRIVATE third_party/glm)
# target_include_directories(${PROJECT_NAME} PRIVATE third_party/stb)
# target_include_directories(${PROJECT_NAME} PRIVATE third_party/imgui)
# target_include_directories(${PROJECT_NAME} PRIVATE third_party/tinygltf)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)