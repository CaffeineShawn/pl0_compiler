cmake_minimum_required(VERSION 3.16)
project(pl0_compiler)

include_directories(.)
set(CMAKE_BUILD_TYPE "Debug")
add_executable(compiler pl0.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
