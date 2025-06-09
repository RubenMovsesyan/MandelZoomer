cmake_minimum_required(VERSION 4.0)

# Change this to the name of application
project(project-template)

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

include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})
