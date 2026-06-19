cmake_minimum_required(VERSION 3.13)

option(ORCA_BUILD_EXAMPLE_PLUGINS "Build example Orca plugin DLLs" OFF)

function(add_orca_example_plugin target_name source_file manifest_dir)
    add_library(${target_name} SHARED ${source_file})
    target_link_libraries(${target_name} PRIVATE OrcaPluginSDK)
    target_compile_definitions(${target_name} PRIVATE ORCA_PLUGIN_EXPORTS)
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${manifest_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${manifest_dir}"
        ARCHIVE_OUTPUT_DIRECTORY "${manifest_dir}"
    )
endfunction()

if (TARGET OrcaPluginSDK AND ORCA_BUILD_EXAMPLE_PLUGINS)
    add_orca_example_plugin(orca_noop noop/noop_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/noop")
    add_orca_example_plugin(orca_round_prime_tower round_prime_tower/round_prime_tower_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/round_prime_tower")
    add_orca_example_plugin(orca_arc_overhang arc_overhang/arc_overhang_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/arc_overhang")
    add_orca_example_plugin(orca_custom_infill custom_infill/custom_infill_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/custom_infill")

    add_custom_command(TARGET orca_noop POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/noop/manifest.json"
            "$<TARGET_FILE_DIR:orca_noop>/manifest.json")
    add_custom_command(TARGET orca_round_prime_tower POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/round_prime_tower/manifest.json"
            "$<TARGET_FILE_DIR:orca_round_prime_tower>/manifest.json")
    add_custom_command(TARGET orca_arc_overhang POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/arc_overhang/manifest.json"
            "$<TARGET_FILE_DIR:orca_arc_overhang>/manifest.json")
    add_custom_command(TARGET orca_custom_infill POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/custom_infill/manifest.json"
            "$<TARGET_FILE_DIR:orca_custom_infill>/manifest.json")
endif()
