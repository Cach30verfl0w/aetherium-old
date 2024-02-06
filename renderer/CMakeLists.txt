project(aetherium-renderer)

# Add dynamic and static library
cmx_add_library(${PROJECT_NAME} SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_kstd_core(${PROJECT_NAME} PUBLIC)
cmx_include_kstd_streams(${PROJECT_NAME} PUBLIC)
cmx_include_kstd_reflect(${PROJECT_NAME} PUBLIC)
cmx_include_phmap(${PROJECT_NAME} PUBLIC)

cmx_add_library(${PROJECT_NAME}-static SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
cmx_include_kstd_core(${PROJECT_NAME}-static PUBLIC)
cmx_include_kstd_streams(${PROJECT_NAME}-static PUBLIC)
cmx_include_kstd_reflect(${PROJECT_NAME}-static PUBLIC)
cmx_include_phmap(${PROJECT_NAME}-static PUBLIC)

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

# Include spdlog
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")
add_compile_definitions(SPDLOG_HEADER_ONLY)

if(${CMX_BUILD_DEBUG})
    message(STATUS "Verbose logging is enabled in debug build")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# Include aetherium core
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../core/include")
target_link_libraries(${PROJECT_NAME} PUBLIC aetherium-core)
target_include_directories(${PROJECT_NAME}-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../core/include")
target_link_libraries(${PROJECT_NAME}-static PUBLIC aetherium-core)

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