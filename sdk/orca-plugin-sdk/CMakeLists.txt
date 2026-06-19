cmake_minimum_required(VERSION 3.13)
project(OrcaPluginSDK VERSION 1.0.0)

add_library(OrcaPluginSDK INTERFACE)
target_include_directories(OrcaPluginSDK INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OrcaPluginSDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/OrcaPluginSDKConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OrcaPluginSDKConfig.cmake"
    @ONLY
)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS OrcaPluginSDK EXPORT OrcaPluginSDKTargets)
install(EXPORT OrcaPluginSDKTargets
    FILE OrcaPluginSDKTargets.cmake
    NAMESPACE OrcaPluginSDK::
    DESTINATION lib/cmake/OrcaPluginSDK
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OrcaPluginSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OrcaPluginSDKConfigVersion.cmake"
    DESTINATION lib/cmake/OrcaPluginSDK
)
