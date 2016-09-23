cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wfatal-errors")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/installs/bin)

add_subdirectory(codegen/cpp)

