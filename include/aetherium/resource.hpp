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

#include "aetherium/utils.hpp"
#include <filesystem>
#include <kstd/option.hpp>
#include <kstd/reflect/reflection.hpp>
#include <kstd/result.hpp>
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <parallel_hashmap/phmap.h>
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <kstd/safe_alloc.hpp>

namespace fs = std::filesystem;

namespace aetherium {
    class ResourceManager;

    class Resource {
        protected:
        fs::path _resource_path;
        const kstd::reflect::RTTI* _runtime_type;

        public:
        explicit Resource(fs::path resource_path, const kstd::reflect::RTTI* runtime_type) noexcept ://NOLINT
                _resource_path {std::move(resource_path)},
                _runtime_type {runtime_type} {
        }
        virtual ~Resource() noexcept = default;

        virtual auto reload(const ResourceManager& resource_manager) noexcept -> kstd::Result<void> {
            UNUSED_PARAMETER(resource_manager);
            return {};
        }

        [[nodiscard]] inline auto get_resource_path() const noexcept -> const fs::path& {
            return _resource_path;
        }

        [[nodiscard]] inline auto get_runtime_type() const noexcept -> const kstd::reflect::RTTI& {
            return *_runtime_type;
        }
    };

    /**
     * This object is a central management unit for all on-filesystem resources. All of the loadable resources can be
     * reloaded.
     *
     * @author Cedric Hammes
     * @since  02/02/2024
     */
    class ResourceManager final {
        phmap::flat_hash_map<std::string, std::shared_ptr<Resource>> _loaded_resources;
        std::string_view _base_directory;

        public:
        /**
         * This constructor creates the resource manager with the default (empty) values
         */
        explicit ResourceManager(std::string_view base_directory) noexcept;
        ~ResourceManager() noexcept = default;
        KSTD_NO_COPY(ResourceManager, ResourceManager);
        KSTD_DEFAULT_MOVE(ResourceManager, ResourceManager);

        /**
         * This function uses the specified space and path to load the resource. After the resource load, the resource
         * manager automatically reloads the resource itself.
         *
         * @param space  The namespace of the resource
         * @param path   The path of the resource
         * @return       The resource itself or an error
         * @author       Cedric Hammes
         * @since        02/02/2024
         */
        template<typename RESOURCE, typename... ARGS>
        [[nodiscard]] auto load_resource(const std::string& space, const std::string& path, ARGS&&... args) noexcept
                -> kstd::Result<RESOURCE&> {
            static_assert(std::is_base_of_v<Resource, RESOURCE>, "Base class of resource isn't Resource");
            using namespace std::string_literals;

            const auto resource_path = fs::path {_base_directory}.append("assets").append(space).append(path);
            if(!fs::exists(resource_path) || !fs::is_regular_file(resource_path)) {
                return kstd::Error {"Unable to load resource: The resource path doesn't exists or isn't a file"s};
            }

            // TODO: Transform construct into result
            const auto rtti = &static_cast<const kstd::reflect::RTTI&>(*kstd::reflect::lookup<RESOURCE>());
            auto resource = std::make_shared<RESOURCE>(resource_path, rtti, std::forward<ARGS>(args)...);
            if(const auto result = resource->reload(*this); result.is_error()) {
                return kstd::Error {result.get_error()};
            }

            _loaded_resources[fmt::format("{}/{}", rtti->to_string(), resource_path.string())] =
                    static_cast<std::shared_ptr<Resource>>(resource);
            return kstd::Result<RESOURCE&> {*resource};
        }

        /**
         * This function enumerates all loaded resources and returns the first resource found or none.
         *
         * @tparam RESOURCE The implementation type of the resource
         * @param space     The resource namespace
         * @param path      The resource path
         * @return          The resource itself or none
         * @author          Cedric Hammes
         * @since           02/02/2024
         */
        template<typename RESOURCE>
        [[nodiscard]] auto get_resource(const std::string& space, const std::string& path) noexcept
                -> kstd::Option<RESOURCE&> {
            static_assert(std::is_base_of_v<Resource, RESOURCE>, "Base class of resource isn't Resource");
            const auto resource_type = static_cast<const kstd::reflect::RTTI*>(&*kstd::reflect::lookup<RESOURCE>());
            const auto resource_path = fs::path {_base_directory}.append("assets").append(space).append(path);
            if(!fs::exists(resource_path) || !fs::is_regular_file(resource_path)) {
                return kstd::Option<RESOURCE&> {};
            }

            const auto identifier = fmt::format("{}/{}", resource_type->to_string(), resource_path.string());
            if(!_loaded_resources.contains(identifier))
                return kstd::Option<RESOURCE&> {};

            return kstd::Option<RESOURCE&> {static_cast<RESOURCE&>(*_loaded_resources[identifier])};
        }

        /**
         * This function tries to get the resource and the resource fetching is failed, the resource manager tries to
         * load the resource by the file.
         *
         * @tparam RESOURCE The resource type
         * @tparam ARGS     The initializer parameters types
         * @param space     The resource namespace
         * @param path      The resource path
         * @param args      The initializer parameters
         * @return          The resource reference or an error
         * @author          Cedric Hammes
         * @since           02/02/2024
         */
        template<typename RESOURCE, typename... ARGS>
        [[nodiscard]] auto get_or_load(const std::string& space, const std::string& path, ARGS&&... args) noexcept
                -> kstd::Result<RESOURCE&> {
            static_assert(std::is_base_of_v<Resource, RESOURCE>, "Base class of resource isn't Resource");
            if(auto result = get_resource<RESOURCE>(space, path); !result.is_empty()) {
                return *result;
            }

            return load_resource<RESOURCE, ARGS...>(space, path, std::forward<ARGS>(args)...);
        }

        /**
         * This function enumerates through all resources with the same type as specified and reloads them.
         *
         * @return The count of reloaded resources or an error
         * @author Cedric Hammes
         * @since  02/02/2024
         */
        template<typename RESOURCE>
        [[nodiscard]] auto reload_by_type() noexcept -> kstd::Result<uint32_t> {
            static_assert(std::is_base_of_v<Resource, RESOURCE>, "Base class of resource isn't Resource");
            const auto resource_type = static_cast<const kstd::reflect::RTTI*>(&*kstd::reflect::lookup<RESOURCE>());

            uint32_t resource_count = 0;
            for(auto& [identifier, resource] : _loaded_resources) {
                if(resource->get_runtime_type().is_same(*resource_type)) {
                    if(const auto result = resource->reload(*this); !result.is_empty()) {
                        return kstd::Error {result.get_error()};
                    }
                    resource_count++;
                }
            }
            return resource_count;
        }

        /**
         * This function enumerates through all resources and reloads them.
         *
         * @return The count of reloaded resources or an error
         * @author Cedric Hammes
         * @since  02/02/2024
         */
        [[nodiscard]] auto reload() noexcept -> kstd::Result<uint32_t>;
    };
}// namespace aetherium