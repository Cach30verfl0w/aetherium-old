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

#include "aetherium/resource.hpp"

namespace aetherium {
    ResourceManager::ResourceManager(std::string_view base_directory) noexcept ://NOLINT
            _base_directory {base_directory} {
    }

    auto ResourceManager::reload() noexcept -> kstd::Result<uint32_t>  {
        for (auto& _loaded_resource : _loaded_resources) {
            if (const auto result = _loaded_resource.second->reload(*this); result.is_error()) {
                return kstd::Error {result.get_error()};
            }
        }
        return _loaded_resources.size();
    }
}// namespace aetherium