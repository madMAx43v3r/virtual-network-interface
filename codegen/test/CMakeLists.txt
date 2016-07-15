cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -pthread -Wfatal-errors")

include_directories(../../cpp/include/ ../../../virtual-network-layers-cpp/include/)

add_executable(test_vni_parser test_vni_parser.cpp)

add_subdirectory(generated)

