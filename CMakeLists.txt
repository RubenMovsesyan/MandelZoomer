cmake_minimum_required(VERSION 4.0)

# Change to release when done
set(CMAKE_BUILD_TYPE Debug)

# Change this to the name of application
project(MandelZoom)

# CMake Package manager
include(FetchContent)

if (APPLE OR CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(GLFW_BUILD_COCOA ON CACHE BOOL "Build GLFW with Cocoa support")
    message(STATUS "=== Setting GLFW_BUILD_COCOA=ON for macOS ===")
endif()


set(LIBS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -g -O0 -DDEBUG")
    message(STATUS "=== DEBUG BUILD ===")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -O3")
    message(STATUS "=== RELEASE BUILD ===")
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all subdirectories in include directory
file(GLOB_RECURSE INCLUDE_SUBDIRS RELATIVE "${INCLUDE_DIR}/*" "${INCLUDE_DIR}")
set(INCLUDE_DIRS_LIST ${INCLUDE_DIR})

foreach(SUBDIR ${INCLUDE_DIRS_LIST})
    if(IS_DIRECTORY ${SUBDIR})
        include_directories(${SUBDIR})
    endif()
endforeach()

file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c")

set(BUILD_SHARED_LIBS OFF)

# Set WebGPU-specific static linking options before FetchContent
set(WEBGPU_BACKEND "DAWN" CACHE STRING "WebGPU backend")
set(WEBGPU_FORCE_SYSTEM_DAWN OFF CACHE BOOL "Force system Dawn")

# WGPU
FetchContent_Declare(
    webgpu
    GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution.git
    GIT_TAG "main"
)

# Set Dawn-specific options for static linking
set(DAWN_FETCH_DEPENDENCIES ON)
set(DAWN_BUILD_SAMPLES OFF)
set(DAWN_ENABLE_INSTALL OFF)
set(DAWN_ENABLE_BACKEND_VULKAN ON)
set(DAWN_ENABLE_BACKEND_METAL ON)
set(DAWN_ENABLE_BACKEND_D3D11 ON)
set(DAWN_ENABLE_BACKEND_D3D12 ON)

FetchContent_MakeAvailable(webgpu)

# GLFW
find_package(glfw3 REQUIRED)

# glfw3webgpu
add_subdirectory(glfw3webgpu)

list(APPEND LIBS webgpu)
list(APPEND LIBS glfw)
list(APPEND LIBS glfw3webgpu)

# macOS-specific system dependencies for SDL3
if (${CMAKE_SYSTEM} MATCHES "Darwin")
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    find_library(CARBON_LIBRARY Carbon)
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)

    # Print each found library
    foreach(lib COCOA_LIBRARY IOKIT_LIBRARY COREVIDEO_LIBRARY COREAUDIO_LIBRARY
                AUDIOTOOLBOX_LIBRARY CARBON_LIBRARY METAL_LIBRARY QUARTZCORE_LIBRARY)
        message(STATUS "${lib}: ${${lib}}")
        if(${lib} MATCHES "NOTFOUND")
            message(WARNING "${lib} not found!")
        endif()
    endforeach()

    list(APPEND LIBS
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${COREAUDIO_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${CARBON_LIBRARY}
        ${METAL_LIBRARY}
        ${QUARTZCORE_LIBRARY}
    )

    message(STATUS "=== Added macOS frameworks ===")
else()
    message(STATUS "=== Not macOS - System: ${CMAKE_SYSTEM_NAME} ===")
endif()

message(STATUS "=== FINAL LIBS LIST ===")
foreach(lib ${LIBS})
    message(STATUS "  - ${lib}")
endforeach()

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})


# Copy WebGPU binaries if needed (for runtime dynamic libraries)
if (TARGET webgpu)
    get_target_property(WEBGPU_TYPE webgpu TYPE)
    if (WEBGPU_TYPE STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:webgpu>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
endif()
