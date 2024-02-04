cmake_minimum_required(VERSION 3.18)
project(aetherium LANGUAGES C CXX)

# Include CMake modules
set(USE_SANITIZER "")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(cmx-bootstrap)
cmx_include_scripts()
include(cmx-glm)
include(cmx-kstd-core)
include(cmx-fmt)
include(cmx-kstd-streams)
include(cmx-kstd-reflect)
include(cmx-parallel-hashmap)

# Shared Library
cmx_add_library(aetherium SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(aetherium PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_glm(aetherium PUBLIC)
cmx_include_kstd_core(aetherium PUBLIC)
cmx_include_kstd_streams(aetherium PUBLIC)
cmx_include_fmt(aetherium PUBLIC)
cmx_include_kstd_reflect(aetherium PUBLIC)
cmx_include_phmap(aetherium PUBLIC)

cmx_add_library(aetherium-static STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(aetherium-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_glm(aetherium-static PUBLIC)
cmx_include_kstd_core(aetherium-static PUBLIC)
cmx_include_kstd_streams(aetherium-static PUBLIC)
cmx_include_fmt(aetherium-static PUBLIC)
cmx_include_kstd_reflect(aetherium-static PUBLIC)
cmx_include_phmap(aetherium-static PUBLIC)

# Unit testing
cmx_add_tests(aetherium-tests "${CMAKE_CURRENT_SOURCE_DIR}/tests")
target_link_libraries(aetherium-tests PRIVATE aetherium-static)
add_dependencies(aetherium-tests aetherium-static)
cmx_include_kstd_core(aetherium-tests)

# Pull in spdlog
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_FMT_EXTERNAL_HO ON)
set(SPDLOG_NO_EXCEPTIONS ON)

FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.x)
FetchContent_Populate(spdlog)

target_include_directories(aetherium PUBLIC "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")
target_include_directories(aetherium-static PUBLIC "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")
add_compile_definitions(SPDLOG_HEADER_ONLY)

if(${CMX_BUILD_DEBUG})
    message(STATUS "Verbose logging is enabled in debug build")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# Pull in volk
FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG master
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(volk)
target_include_directories(aetherium PUBLIC "${CMAKE_BINARY_DIR}/_deps/volk-src/include")
target_link_libraries(aetherium PUBLIC volk)
target_include_directories(aetherium-static PRIVATE "${CMAKE_BINARY_DIR}/_deps/volk-src/include")
target_link_libraries(aetherium-static PUBLIC volk)

# Pull in SDL2
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG SDL2
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(SDL2)
target_include_directories(aetherium PUBLIC "${CMAKE_BINARY_DIR}/_deps/sdl2-src/include")
target_link_libraries(aetherium PUBLIC SDL2 SDL2main)
target_include_directories(aetherium-static PRIVATE "${CMAKE_BINARY_DIR}/_deps/sdl2-src/include")
target_link_libraries(aetherium-static PUBLIC SDL2 SDL2main)

# Pull in spirv-headers
FetchContent_Declare(
        SPIRV-Headers
        GIT_REPOSITORY https://github.com/KhronosGroup/spirv-headers.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(SPIRV-Headers)

# Pull in spirv-tools
FetchContent_Declare(
        SPIRV-Tools
        GIT_REPOSITORY https://github.com/KhronosGroup/spirv-tools.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(SPIRV-Tools)

# Pull in glsllang
FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(glslang)

# Pull in shaderc
FetchContent_Declare(
        shaderc
        GIT_REPOSITORY https://github.com/google/shaderc.git
        GIT_TAG main
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(shaderc)
target_include_directories(aetherium PUBLIC "${CMAKE_BINARY_DIR}/_deps/spirv-cross-src/include")
target_link_libraries(aetherium PUBLIC shaderc)
target_include_directories(aetherium-static PUBLIC "${CMAKE_BINARY_DIR}/_deps/spirv-cross-src/include")
target_link_libraries(aetherium-static PUBLIC shaderc)

# Add example folders
message("Adding example projects as applications")
file(GLOB EXAMPLE_FOLDER_LIST DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/examples/*")
foreach(EXAMPLE_FOLDER ${EXAMPLE_FOLDER_LIST})
    get_filename_component(PROJECT_NAME ${EXAMPLE_FOLDER} NAME)
    cmx_add_application(${PROJECT_NAME} "${EXAMPLE_FOLDER}")
    message(STATUS "Add ${PROJECT_NAME} (${EXAMPLE_FOLDER}) as example project")
    target_link_libraries(${PROJECT_NAME} PUBLIC aetherium)
    add_compile_definitions(${PROJECT_NAME} EXAMPLE_APP_PATH="${EXAMPLE_FOLDER}")
endforeach()
