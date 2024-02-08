cmake_minimum_required(VERSION 3.18)
project(aetherium LANGUAGES C CXX)

# Include CMake Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(cmx-bootstrap)
include(cmx-kstd-core)
include(cmx-kstd-reflect)
include(cmx-parallel-hashmap)
include(cmx-sdl2)
include(cmx-spdlog)
cmx_include_scripts()

set(CMX_SDL2_VERSION SDL2)

# Library
cmx_add_library(${PROJECT_NAME} SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_kstd_core(${PROJECT_NAME} PUBLIC)
cmx_include_kstd_reflect(${PROJECT_NAME} PUBLIC)
cmx_include_phmap(${PROJECT_NAME} PUBLIC)
cmx_include_sdl2(${PROJECT_NAME} PUBLIC)
cmx_include_spdlog(${PROJECT_NAME} PUBLIC)

cmx_add_library(${PROJECT_NAME}-static STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_kstd_core(${PROJECT_NAME}-static PUBLIC)
cmx_include_kstd_reflect(${PROJECT_NAME}-static PUBLIC)
cmx_include_phmap(${PROJECT_NAME}-static PUBLIC)
cmx_include_sdl2(${PROJECT_NAME}-static PUBLIC)
cmx_include_spdlog(${PROJECT_NAME}-static PUBLIC)

# Include SPIR-V headers
FetchContent_Declare(
        spirv-headers
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(spirv-headers)

# Include SPIR-V tools
FetchContent_Declare(
        spirv-tools
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(spirv-tools)

# Include glslang
FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(glslang)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/_deps/glslang-src/include")
target_link_libraries(${PROJECT_NAME} PUBLIC glslang)
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_BINARY_DIR}/_deps/glslang-src/include")
target_link_libraries(${PROJECT_NAME}-static PUBLIC glslang)

# Include volk
FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG master
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(volk)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/_deps/volk-src/include")
target_link_libraries(${PROJECT_NAME} PUBLIC volk)
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_BINARY_DIR}/_deps/volk-src/include")
target_link_libraries(${PROJECT_NAME}-static PUBLIC volk)

# Include unit tests
cmx_add_tests(${PROJECT_NAME}-tests "${CMAKE_CURRENT_SOURCE_DIR}/tests")
cmx_include_kstd_core(${PROJECT_NAME}-tests)
target_compile_definitions(${PROJECT_NAME}-tests PRIVATE TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/tests")
target_link_libraries(${PROJECT_NAME}-tests PRIVATE ${PROJECT_NAME}-static)
add_dependencies(${PROJECT_NAME}-tests ${PROJECT_NAME}-static)

# Example application
cmx_add_application(simple-window-example "${CMAKE_CURRENT_SOURCE_DIR}/examples/simple-window-example/")
target_link_libraries(simple-window-example PRIVATE ${PROJECT_NAME}-static)
add_dependencies(simple-window-example ${PROJECT_NAME}-static)