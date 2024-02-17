// Copyright 2024 Cedric Hammes/Cach30verfl0w
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "aetherium/renderer/shader.hpp"

// TODO: Getter for IR array and * operator overload for Vulkan shader module

namespace aetherium::renderer {
    Shader::~Shader() noexcept {
        if(_compiler != nullptr) {
            shaderc_compiler_release(_compiler);
            _compiler = nullptr;
        }
    }

    auto Shader::reload(const aetherium::ResourceManager& resource_manager) noexcept -> kstd::Result<void> {
        UNUSED_PARAMETER(resource_manager);
        // TODO: Read file data

        auto* compiler = shaderc_compiler_initialize();
        auto* compile_options = shaderc_compile_options_initialize();
        shaderc_compile_options_set_source_language(compile_options, shaderc_source_language_glsl);
        // TODO: Compile file data into shader
        shaderc_compile_options_release(compile_options);

        // TODO: Create vulkan shader module by IR and store IR in module
        return {};
    }
}// namespace aetherium::renderer
