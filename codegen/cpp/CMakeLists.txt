cmake_minimum_required(VERSION 2.4)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wfatal-errors")

include_directories(../)

add_executable(vnicppcodegen vnicppcodegen.cpp)

INSTALL(TARGETS vnicppcodegen
  RUNTIME DESTINATION bin
)

