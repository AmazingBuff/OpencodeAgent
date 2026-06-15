#
# tests/CMakeLists.txt
#

function(add_test_target test_name)
    file(GLOB_RECURSE source_files ${test_name}/*.cpp)

    add_executable(${test_name})
    target_sources(${test_name} PRIVATE ${source_files})
    target_include_directories(
        ${test_name}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}
    )
    target_link_libraries(
        ${test_name}
        PRIVATE
        ${PROJECT_NAME}
    )
    target_precompile_headers(
        ${test_name}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/pch.h
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

enable_testing()

add_test_target(test_core)
