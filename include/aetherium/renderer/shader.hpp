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
#include <shaderc/shaderc.hpp>

namespace aetherium::renderer {
    class Shader final : public Resource {
        shaderc_compiler* _compiler;

        public:
        Shader(const fs::path& resource_path, const kstd::reflect::RTTI* runtime_type) :
                Resource {resource_path, runtime_type} {
            _compiler = shaderc_compiler_initialize();
        }
        ~Shader() noexcept override;
        KSTD_NO_MOVE_COPY(Shader, Shader);

        auto reload(const aetherium::ResourceManager& resource_manager) noexcept -> kstd::Result<void> override;
    };
}// namespace aetherium::renderer
