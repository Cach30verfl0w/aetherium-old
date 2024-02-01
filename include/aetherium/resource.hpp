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
    /**
     * The resource identifies a single resource, managed by the resource manager itself. These resources are used by
     * the resource manager as a container of the type and path together, so the manager can identify the resource by
     * these values.
     *
     * @tparam RESOURCE_TYPE Enum type used for the identification of the resource group
     * @since               01/02/2024
     * @author              Cedric Hammes
     */
    template<typename RESOURCE_TYPE>
    class Resource final {
        static_assert(std::is_enum_v<RESOURCE_TYPE>, "Resource Type is not enum type");

        RESOURCE_TYPE _resource_type;
        fs::path _resource_path;

        public:
        /**
         * This constructor creates the resource by the specified data
         *
         * @param resource_type The type of the resource
         * @param resource_path The path to the resource
         * @since               01/02/2024
         * @author              Cedric Hammes
         */
        Resource(RESOURCE_TYPE resource_type, fs::path resource_path) noexcept :
                _resource_type {resource_type},
                _resource_path {std::move(resource_path)}
        {}

        /**
         * This function returns th type of the resource.
         *
         * @return The type of the resource
         * @since  01/02/2024
         * @author Cedric Hammes
         */
        [[nodiscard]] inline auto get_resource_type() const noexcept -> RESOURCE_TYPE {
            return _resource_type;
        }

        /**
         * This function returns a pointer to the resource path of the resource.
         *
         * @return The pointer to the resource path
         * @since  01/02/2024
         * @author Cedric Hammes
         */
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

    /**
     * The resource manager is an implementation of a central management of resource files at runtime. This allows the
     * developer to add directories into a resource group and load the single resources with a factory.
     *
     * @tparam RESOURCE_TYPE Enum type used for the identification of resource groups/categories
     * @since                01/02/2024
     * @author               Cedric Hammes
     */
    template<typename RESOURCE_TYPE>
    class ResourceManager final {
        static_assert(std::is_enum_v<RESOURCE_TYPE>, "Resource Type is not enum type");

        std::vector<Resource<RESOURCE_TYPE>> _registered_resources {};
        std::unordered_map<RESOURCE_TYPE, fs::path> _reload_paths {};

        public:
        /**
         * This constructor creates the resource manager with the default (empty) values
         */
        ResourceManager() noexcept = default;

        /**
         * This function allows the developer to add the specified directory into the specified resource group. So the
         * resource manager can index the files and reload the groups at reload.
         *
         * @param type      The resource group of the specified resource directory
         * @param directory The path to the resource directory of the resource group
         * @return          The result to validate the success of this operation and perform error handling
         *
         * @since           01/02/2024
         * @author          Cedric Hammes
         */
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

        /**
         * This function allows the developer to reload the resource manager for a specific resource group or all
         * registered resource groups. This mechanic re-enumerates all registered directories and add the files
         * into it.
         *
         * @param type The optional resource group to reload
         * @return     The result to validate the success of this operation and perform error handling
         *
         * @since      01/02/2024
         * @author     Cedric Hammes
         */
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

        /**
         * This function enumerates and loads all resources by the specified group.
         *
         * @tparam TYPE   The type of the resource group
         * @tparam F      The type of the factory function
         * @tparam FR     The return type of the factory function
         * @param factory The factory function as parameter
         * @return        The list of all created resources or an error
         *
         * @since         01/02/2024
         * @author        Cedric Hammes
         */
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

        /**
         * This function enumerates and loads the specified resource by the name in the group.
         *
         * @tparam TYPE   The type of the resource group
         * @tparam F      The type of the factory function
         * @tparam FR     The return type of the factory function
         * @param name    The name of the wanted resource
         * @param factory The factory function as parameter
         * @return        The list of all created resources or an error
         *
         * @since         01/02/2024
         * @author        Cedric Hammes
         */
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