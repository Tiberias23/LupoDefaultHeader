cmake_minimum_required(VERSION 3.15)
project(lupo_default_header)

add_library(lupo_default_header STATIC lupo_default_header.cpp)
target_include_directories(lupo_default_header PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# `errors_as_values.h` is a header-only utility. Do not create a STATIC library from a
# header file — that would make CMake try to compile the header. Use an INTERFACE
# library instead and expose the include directory.
add_library(errors_as_values INTERFACE)
target_include_directories(errors_as_values INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
