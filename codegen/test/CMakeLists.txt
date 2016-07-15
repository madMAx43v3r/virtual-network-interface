cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -O2 -pthread -Wfatal-errors")

include_directories(
	generated/include/
	../../cpp/include/
	../../../virtual-network-layers-cpp/include/
)

link_directories(../../../virtual-network-layers-cpp/lib/)

add_executable(test_module test_module.cpp)
target_link_libraries(test_module interface vnl)

add_subdirectory(generated)

