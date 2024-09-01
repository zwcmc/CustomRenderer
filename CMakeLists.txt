cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME CustomRenderer)

message(STATUS "CMake Generator is: ${CMAKE_GENERATOR}")
if ("${CMAKE_GENERATOR}" STREQUAL "Xcode")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "-std=c++17")
    add_definitions(-DXCODE_PROJECT=1)
endif()

project(${PROJECT_NAME})

set(CMAKE_USE_RELATIVE_PATHS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Helper function to create source groups
function(create_source_groups base_dir)
    file(GLOB_RECURSE ALL_FILES
        "${base_dir}/*.cpp"
        "${base_dir}/*.c"
        "${base_dir}/*.h"
    )

    foreach(FILE_PATH ${ALL_FILES})
        file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" "${FILE_PATH}")
        get_filename_component(FILE_DIR "${REL_PATH}" PATH)
        string(REPLACE "\\" "/" GROUP_PATH "${FILE_DIR}")
        source_group("${GROUP_PATH}" FILES "${FILE_PATH}")
    endforeach()
endfunction()

# Set the base directory for source files
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BASE_GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad")
set(BASE_STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb")
set(BASE_IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

# Create source groups
create_source_groups(${BASE_DIR})
create_source_groups(${BASE_GLAD_DIR})
create_source_groups(${BASE_STB_DIR})
create_source_groups(${BASE_IMGUI_DIR})

# Collect all source files
file(GLOB_RECURSE PROJECT_SOURCES
    "${BASE_DIR}/*.h"
    "${BASE_DIR}/*.cpp"
    "${BASE_GLAD_DIR}/include/*.h"
    "${BASE_GLAD_DIR}/src/*.c"
    "${BASE_STB_DIR}/*.h"
    "${BASE_STB_DIR}/*.cpp"
    "${BASE_IMGUI_DIR}/*.h"
    "${BASE_IMGUI_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(OpenGL_LIBRARY OpenGL REQUIRED)
    find_library(IOKit_LIBRARY IOKit REQUIRED)
    find_library(CoreVideo_LIBRARY CoreVideo REQUIRED)
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

# assimp
add_subdirectory(third_party/assimp)
target_link_libraries(${PROJECT_NAME} assimp)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/stb)
target_include_directories(${PROJECT_NAME} PRIVATE third_party/imgui)