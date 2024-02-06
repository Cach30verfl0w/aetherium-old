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
#include "aetherium/renderer/shader.hpp"

namespace aetherium::renderer {
    auto Shader::reload(const aetherium::core::ResourceManager& resource_manager) noexcept -> kstd::Result<void> {
        UNUSED_PARAMETER(resource_manager);
    }
}