cmake_minimum_required(VERSION 3.20.x)

project(reg_helper VERSION 0.0.1)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/include/")