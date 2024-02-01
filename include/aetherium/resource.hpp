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

#define once
#include <filesystem>
#include <fstream>
#include <kstd/option.hpp>
#include <kstd/result.hpp>
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace aetherium {
    template<typename RESOURCE_TYPE>
    class Resource final {
        RESOURCE_TYPE _resource_type;
        fs::path _resource_path;

        public:
        Resource(RESOURCE_TYPE resource_type, fs::path resource_path) noexcept :
                _resource_type {resource_type},
                _resource_path {std::move(resource_path)}
        {}

        [[nodiscard]] inline auto get_resource_type() const noexcept -> RESOURCE_TYPE {
            return _resource_type;
        }

        [[nodiscard]] inline auto get_resource_path() const noexcept -> const fs::path* {
            return &_resource_path;
        }
    };

    constexpr auto passthough_factory = [](auto& value) noexcept -> auto {
        return value;
    };

    constexpr auto read_string_factory = [](auto& value) noexcept -> auto {
        std::ifstream file {value.get_resource_path()->c_str()};
        return std::string {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    };

    template<typename RESOURCE_TYPE>
    class ResourceManager final {
        static_assert(std::is_enum_v<RESOURCE_TYPE>, "Resource Type is not enum type");
        std::vector<Resource<RESOURCE_TYPE>> _registered_resources {};
        std::unordered_map<RESOURCE_TYPE, fs::path> _reload_paths {};

        public:
        ResourceManager() noexcept = default;

        [[nodiscard]] auto add_directory(const RESOURCE_TYPE type, const std::string_view directory) noexcept -> kstd::Result<void> {
            using namespace std::string_literals;
            const auto resource_directory_path = fs::path {directory};
            if (!(fs::exists(resource_directory_path) || fs::is_directory(resource_directory_path))) {
                return kstd::Error {"Unable to add resource directory: Path doesn't exists or isn't a directory"s};
            }

            SPDLOG_DEBUG("Resource Manager: Including directory '{}' as resource directory, enumerating entries", directory);
            for (const auto& resource_path : fs::directory_iterator(resource_directory_path)) {
                SPDLOG_DEBUG("Resource Manager: Registering path '{}' resource", resource_path.path().c_str());
                _registered_resources.emplace_back(type, resource_path);
            }
            _reload_paths.insert({type, resource_directory_path});
            return {};
        }

        [[nodiscard]] auto reload_resources(const kstd::Option<RESOURCE_TYPE> type) noexcept -> kstd::Result<uint32_t> {
            if (type.has_value()) {
                SPDLOG_DEBUG("Resource Manager: Reloading all resource manager's resources");
                std::remove_if(_registered_resources.begin(), _registered_resources.end(), [&](const auto& resource) {
                    return resource.get_resource_type() == type.get();
                });
            } else {
                SPDLOG_DEBUG("Resource Manager: Reloading specific resources by type/category");
                _registered_resources.clear();
            }

            for(const auto& [resource_dir_type, resource_directory] : _reload_paths) {
                if (type.map([&](const auto value) { return value == type; }).get_or(true))
                    continue;

                add_directory(type, resource_directory);
            }
            return {};
        }

        template<RESOURCE_TYPE TYPE, typename F, typename FR = std::invoke_result_t<F, Resource<RESOURCE_TYPE>&>>
        [[nodiscard]] auto load_resources(F&& factory) noexcept -> kstd::Result<std::vector<FR>> {
            static_assert(std::is_convertible_v<F, std::function<kstd::Result<FR>(Resource<RESOURCE_TYPE>&)>>,
                          "Factory signature does not match");
            using namespace std::string_literals;
            std::vector<FR> resource_list {};
            for (const auto& resource : _registered_resources) {
                if (resource.get_resource_type() != TYPE)
                    continue;

                const kstd::Result<FR> result = factory(resource);
                if (result.is_error()) {
                    return kstd::Error {result.get_error()};
                }

                resource_list.push_back(result.get());
            }
            return resource_list;
        }

        template<RESOURCE_TYPE TYPE, typename F, typename FR = std::invoke_result_t<F, Resource<RESOURCE_TYPE>&>>
        [[nodiscard]] auto load_resource(std::string_view name, F&& factory) noexcept -> kstd::Result<FR> {
            static_assert(std::is_convertible_v<F, std::function<kstd::Result<FR>(Resource<RESOURCE_TYPE>&)>>,
                          "Factory signature does not match");
            using namespace std::string_literals;

            const auto opt_result_value = kstd::streams::stream(_registered_resources)
                    .filter([&](auto& resource) {
                        return resource.get_resource_type() == TYPE && resource.get_resource_path()->filename() == name;
                    })
                    .map(factory)
                    .find_any();
            if (opt_result_value.is_empty()) {
                return kstd::Error { fmt::format("Resource Manager: No resource named '{}' found", name) };
            }

            return opt_result_value.get();
        }

    };
}