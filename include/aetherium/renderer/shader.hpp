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

#pragma once

#include "aetherium/resource.hpp"
#include "aetherium/utils.hpp"
#include <fstream>
#include <shaderc/shaderc.hpp>

namespace aetherium::renderer {
    enum ShaderKind {
        FRAGMENT,
        VERTEX
    };

    namespace {
        [[nodiscard]] constexpr auto shader_kind_into_shaderc(const ShaderKind shader_kind) -> shaderc_shader_kind {
            switch(shader_kind) {
                case ShaderKind::VERTEX: return shaderc_vertex_shader;
                case ShaderKind::FRAGMENT: return shaderc_fragment_shader;
            }
        }
    }// namespace

    class Shader : public Resource {
        const ShaderKind _shader_kind;
        std::vector<uint8_t> _bytecode;

        public:
        explicit Shader(fs::path path, const kstd::reflect::RTTI* runtime_type, const ShaderKind shader_kind) ://NOLINT
                Resource {std::move(path), runtime_type},
                _shader_kind {shader_kind} {
        }

        kstd::Result<void> reload(const aetherium::ResourceManager& resource_manager) noexcept final;
    };
}// namespace aetherium::renderer