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

namespace aetherium::renderer {
    auto Shader::reload(const ResourceManager& resource_manager) noexcept -> kstd::Result<void> {
        UNUSED_PARAMETER(resource_manager);

        auto stream = std::ifstream {_resource_path};
        stream.unsetf(std::ios::skipws);

        const std::string data {std::istream_iterator<char> {stream}, std::istream_iterator<char> {}};
        const auto shader_name = _resource_path.filename().c_str();

        // Parse and compile IR
        auto* compiler = shaderc_compiler_initialize();
        auto* compile_options = shaderc_compile_options_initialize();
        shaderc_compile_options_set_source_language(compile_options, shaderc_source_language_glsl);

        auto* result = shaderc_compile_into_spv(compiler, data.data(), data.size(),
                                                shader_kind_into_shaderc(_shader_kind), shader_name, "main", nullptr);
        if(const auto status = shaderc_result_get_compilation_status(result);
           status != shaderc_compilation_status_success) {
            return kstd::Error {fmt::format("Unable to reload shader: {}", shaderc_result_get_error_message(result))};
        }

        _bytecode = {};
        _bytecode.reserve(shaderc_result_get_length(result));
        std::memcpy(_bytecode.data(), shaderc_result_get_bytes(result), _bytecode.size());
        shaderc_result_release(result);
        shaderc_compile_options_release(compile_options);
        shaderc_compiler_release(compiler);
        return {};
    }
}// namespace aetherium::renderer