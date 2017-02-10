cmake_minimum_required(VERSION 2.4.0 FATAL_ERROR)

set(TARGET_NAME vnicppcodegen)

project(${TARGET_NAME}Project)

add_executable(${TARGET_NAME} vnicppcodegen.cpp)

target_include_directories(${TARGET_NAME} PRIVATE ../)

target_compile_options(${TARGET_NAME} PUBLIC -std=c++11)

install(TARGETS ${TARGET_NAME} DESTINATION bin)
