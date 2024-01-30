cmake_minimum_required(VERSION 3.18)
project(aetherium LANGUAGES C CXX)

# Include packages
find_package(Vulkan REQUIRED)

# Include CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(cmx-bootstrap)
cmx_include_scripts()
include(cmx-glm)
include(cmx-kstd-core)
include(cmx-fmt)

# Shared Library
cmx_add_library(aetherium SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(aetherium PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(aetherium PUBLIC Vulkan::Vulkan)
cmx_include_glm(aetherium PUBLIC)
cmx_include_kstd_core(aetherium PUBLIC)
cmx_include_fmt(aetherium PUBLIC)

cmx_add_library(aetherium-static STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(aetherium-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(aetherium-static PRIVATE Vulkan::Vulkan)
cmx_include_glm(aetherium-static PRIVATE)
cmx_include_kstd_core(aetherium-static PRIVATE)
cmx_include_fmt(aetherium-static PRIVATE)

# Unit testing
cmx_add_tests(aetherium-tests "${CMAKE_CURRENT_SOURCE_DIR}/test")
target_link_libraries(aetherium-tests PRIVATE aetherium-static)
add_dependencies(aetherium-tests aetherium-static)
cmx_include_kstd_core(aetherium-tests PRIVATE)