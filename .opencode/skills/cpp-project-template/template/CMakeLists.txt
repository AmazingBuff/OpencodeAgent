cmake_minimum_required(VERSION 3.25)

project(
    {{PROJECT_NAME}}
    LANGUAGES CXX C
    DESCRIPTION "{{PROJECT_DESCRIPTION}}"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler detection and options
if(MSVC)
    add_compile_options(/W4 /utf-8)
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Subdirectories
add_subdirectory(ext)
add_subdirectory(3rd)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

# Source files
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/{{PROJECT_NAME}}/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/pch\\.cpp$")

# Main library target
add_library({{PROJECT_NAME}})

target_sources(
    {{PROJECT_NAME}}
    PRIVATE
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    {{PROJECT_NAME}}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_precompile_headers(
    {{PROJECT_NAME}}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h
)

target_link_libraries(
    {{PROJECT_NAME}}
    PUBLIC
    ${THIRD_PARTY_LIBS}
)

target_compile_features(
    {{PROJECT_NAME}}
    PUBLIC
    cxx_std_23
)

target_compile_definitions(
    {{PROJECT_NAME}}
    PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)
