cmake_minimum_required(VERSION 4.0)

# Change this to the name of application
project(MandelZoom)

# CPM
# Package manager for CMAKE
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)


set(LIBS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -O3")

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

file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/*.cpp")

set(BUILD_SHARED_LIBS OFF)

# SDL
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
CPMAddPackage("gh:libsdl-org/SDL#4c70aa3")
list(APPEND LIBS SDL3::SDL3)

# macOS-specific system dependencies for SDL3
if (${CMAKE_SYSTEM} MATCHES "Darwin")
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    find_library(CARBON_LIBRARY Carbon)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)

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
endif()


add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
