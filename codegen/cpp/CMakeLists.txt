cmake_minimum_required(VERSION 2.4)

include_directories(../)

add_executable(vnicppcodegen vnicppcodegen.cpp)

INSTALL(TARGETS vnicppcodegen
  RUNTIME DESTINATION bin
)

