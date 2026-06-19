cmake_minimum_required(VERSION 3.13)
project(orca_plugin_template)

find_package(OrcaPluginSDK REQUIRED)

add_library(${PROJECT_NAME} SHARED src/plugin.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE OrcaPluginSDK)
target_compile_definitions(${PROJECT_NAME} PRIVATE ORCA_PLUGIN_EXPORTS)
