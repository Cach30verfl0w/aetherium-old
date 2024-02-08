cmake_minimum_required(VERSION 3.18)
project(aetherium LANGUAGES C CXX)

# Include CMake Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(cmx-bootstrap)
include(cmx-kstd-core)
include(cmx-kstd-reflect)
include(cmx-parallel-hashmap)
cmx_include_scripts()

# Pull in spdlog
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_FMT_EXTERNAL_HO ON)
set(SPDLOG_NO_EXCEPTIONS ON)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
        GIT_PROGRESS true
)
FetchContent_Populate(spdlog)

# Include unit tests
cmx_add_tests(${PROJECT_NAME}-tests "${CMAKE_CURRENT_SOURCE_DIR}/tests")
cmx_include_kstd_core(${PROJECT_NAME}-tests)
target_compile_definitions(${PROJECT_NAME}-tests PRIVATE TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Include modules
message("Include modules/subprojects")
file(GLOB PROJECTS "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(PROJECT ${PROJECTS})
    if (EXISTS "${PROJECT}/CMakeLists.txt")
        get_filename_component(MODULE_NAME ${PROJECT} NAME)
        message(STATUS "Add subproject ${MODULE_NAME} (${PROJECT})")
        add_subdirectory(${PROJECT})
        target_link_libraries(${PROJECT_NAME}-tests PRIVATE aetherium-${MODULE_NAME}-static)
        add_dependencies(${PROJECT_NAME}-tests aetherium-${MODULE_NAME}-static)
    endif()
endforeach()

# Include examples
message("Include examples")
file(GLOB EXAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples/*")
foreach(EXAMPLE_DIR ${EXAMPLES_DIR})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
    message(STATUS "Add example ${EXAMPLE_NAME} (${EXAMPLE_DIR})")
    add_subdirectory(${EXAMPLE_DIR})
    target_compile_definitions(${EXAMPLE_NAME} PRIVATE EXAMPLE_DIRECTORY="${EXAMPLE_DIR}")
endforeach()