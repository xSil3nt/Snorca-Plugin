cmake_minimum_required(VERSION 3.13)

project(nanosvg)

add_library(nanosvg STATIC nanosvg_impl.cpp)

target_include_directories(nanosvg SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
