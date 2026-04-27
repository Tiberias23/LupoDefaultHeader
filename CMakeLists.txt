cmake_minimum_required(VERSION 3.15)
project(lupo_default_header)

add_library(lupo_default_header STATIC lupo_default_header.cpp)
target_include_directories(lupo_default_header PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)