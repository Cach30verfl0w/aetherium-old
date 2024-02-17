include_guard()

set(CMX_SHADERC_VERSION main)
set(CMX_SHADERC_FETCHED OFF)

set(CMX_SPIRV_HEADERS_VERSION main)
set(CMX_SPIRV_HEADERS_FETCHED OFF)

set(CMX_SPIRV_TOOLS_VERSION main)
set(CMX_SPIRV_TOOLS_FETCHED OFF)

macro(cmx_include_shaderc target)
    set(num_args ${ARGC})
    if (num_args GREATER 1)
        set(access ${ARGV1}) # Copy first optional argument
    else ()
        set(access PUBLIC) # Default to PUBLIC
    endif ()

    if (NOT CMX_SHADERC_FETCHED)
        if (NOT TARGET SPIRV-Headers)
            FetchContent_Declare(
                    SPIRV-Headers
                    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
                    GIT_TAG ${CMX_SPIRV_HEADERS_VERSION}
            )
            FetchContent_MakeAvailable(SPIRV-Headers)
            set(CMX_SPIRV_HEADERS_FETCHED ON)
        endif()

        if (NOT TARGET SPIRV-Tools)
            FetchContent_Declare(
                    SPIRV-Tools
                    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
                    GIT_TAG ${CMX_SPIRV_TOOLS_VERSION}
            )
            FetchContent_MakeAvailable(SPIRV-Tools)
            set(CMX_SPIRV_TOOLS_FETCHED ON)
        endif()

        if (NOT TARGET glslang)
            FetchContent_Declare(
                    glslang
                    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
                    GIT_TAG ${CMX_SPIRV_TOOLS_VERSION}
            )
            FetchContent_MakeAvailable(glslang)
            set(CMX_GLSLANG_FETCHED ON)
        endif()

        set(SHADERC_SKIP_TESTS ON)
        set(SHADERC_ENABLE_TESTS OFF)
        FetchContent_Declare(
                shaderc
                GIT_REPOSITORY https://github.com/google/shaderc.git
                GIT_TAG ${CMX_SHADERC_VERSION}
        )
        FetchContent_MakeAvailable(shaderc)
        set(CMX_SHADERC_FETCHED ON)
    endif () # CMX_SHADERC_FETCHED
    target_link_libraries(${target} ${access} shaderc)
    add_dependencies(${target} shaderc)
endmacro()
