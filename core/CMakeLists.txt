project(aetherium-core)

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