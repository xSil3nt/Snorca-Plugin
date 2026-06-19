cmake_minimum_required(VERSION 3.13)
project(OrcaPluginSDK VERSION 2.0.0)

add_library(OrcaPluginSDK INTERFACE)
target_include_directories(OrcaPluginSDK INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (EIGEN3_INCLUDE_DIR AND EXISTS "${EIGEN3_INCLUDE_DIR}/Eigen/Core")
    target_include_directories(OrcaPluginSDK SYSTEM INTERFACE
        $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIR}>
    )
elseif (EXISTS "${CMAKE_SOURCE_DIR}/deps_src/eigen/Eigen/Core")
    target_include_directories(OrcaPluginSDK SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/deps_src/eigen>
    )
elseif (TARGET Eigen3::Eigen)
    target_link_libraries(OrcaPluginSDK INTERFACE Eigen3::Eigen)
else()
    find_package(Eigen3 REQUIRED)
    target_link_libraries(OrcaPluginSDK INTERFACE Eigen3::Eigen)
endif()

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
