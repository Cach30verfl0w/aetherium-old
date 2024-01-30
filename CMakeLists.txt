cmake_minimum_required(VERSION 3.18)
project(example LANGUAGES C CXX)

# Include CMake modules in ./cmake/
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(cmx-bootstrap)
cmx_include_scripts() # Enable sanitizers on supported platforms and extend compiler warnings

# Shared Library
cmx_add_library(example SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(example PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

cmx_add_library(example-static STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(example-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Unit testing
cmx_add_tests(example-tests "${CMAKE_CURRENT_SOURCE_DIR}/test")
target_link_libraries(example-tests PRIVATE example-static)
add_dependencies(example-tests example-static)
