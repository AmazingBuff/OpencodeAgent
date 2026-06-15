#
# examples/CMakeLists.txt
#

function(add_sample sample_name)
    file(GLOB_RECURSE source_files ${sample_name}/*.cpp)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_BINARY_DIR}/${sample_name})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/${sample_name})

    add_executable(${sample_name})
    target_sources(${sample_name} PRIVATE ${source_files})
    target_link_libraries(
        ${sample_name}
        PRIVATE
        ${PROJECT_NAME}
    )
    target_precompile_headers(
        ${sample_name}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/pch.h
    )

    add_custom_command(
        TARGET
        ${sample_name}
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${THIRD_PARTY_DLLS} $<TARGET_FILE_DIR:${sample_name}>
        VERBATIM
    )
endfunction()

function(add_samples sample_list)
    foreach(sample ${sample_list})
        add_sample(${sample})
    endforeach()
endfunction()

# Add samples here:
add_sample(hello_world)
