cmake_minimum_required(VERSION 3.13)

function(add_orca_example_plugin target_name source_file manifest_dir)
    add_library(${target_name} SHARED ${source_file})
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/sdk/orca-plugin-sdk/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/libslic3r
    )
    target_link_libraries(${target_name} PRIVATE libslic3r)
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${manifest_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${manifest_dir}"
        ARCHIVE_OUTPUT_DIRECTORY "${manifest_dir}"
    )
endfunction()

if (TARGET libslic3r)
    add_orca_example_plugin(orca_noop noop/noop_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/noop")
    add_orca_example_plugin(orca_round_prime_tower round_prime_tower/round_prime_tower_plugin.cpp "${CMAKE_CURRENT_SOURCE_DIR}/round_prime_tower")

    add_custom_command(TARGET orca_noop POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/noop/manifest.json"
            "$<TARGET_FILE_DIR:orca_noop>/manifest.json")
    add_custom_command(TARGET orca_round_prime_tower POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/round_prime_tower/manifest.json"
            "$<TARGET_FILE_DIR:orca_round_prime_tower>/manifest.json")
endif()
