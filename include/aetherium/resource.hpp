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
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace aetherium {
    class ResourceManager;

    class Resource {
        protected:
        fs::path _resource_path;

        public:
        explicit Resource(fs::path resource_path) noexcept;
        virtual ~Resource() noexcept = default;

        virtual auto reload(const ResourceManager& resource_manager) noexcept -> kstd::Result<void> {
            return {};
        }
    };

    class ResourceManager final {
        std::vector<std::shared_ptr<Resource>> _loaded_resources;
        std::string_view _base_directory;

        public:
        /**
         * This constructor creates the resource manager with the default (empty) values
         */
        explicit ResourceManager(std::string_view base_directory) noexcept;
        KSTD_NO_COPY(ResourceManager, ResourceManager);
        KSTD_DEFAULT_MOVE(ResourceManager, ResourceManager);

        /**
         * This function uses the specified space and path to load the resource. After the resource load, the resource
         * manager automatically reloads the resource itself.
         *
         * @param space  The namespace of the resource
         * @param path   The path of the resource
         * @return       The resource itself or an error
         */
        template<typename RESOURCE, typename... ARGS>
        [[nodiscard]] auto load_resource(const std::string& space, const std::string& path, ARGS&&... args) noexcept
                -> kstd::Result<std::shared_ptr<RESOURCE>> {
            static_assert(std::is_base_of_v<Resource, RESOURCE>, "Base class of resource isn't Resource");
            using namespace std::string_literals;

            const auto resource_path = fs::path {_base_directory}.append("assets").append(space).append(path);
            if(!fs::exists(resource_path) || !fs::is_regular_file(resource_path)) {
                return kstd::Error {"Unable to load resource: The resource path doesn't exists or isn't a file"s};
            }

            auto resource = std::make_shared<RESOURCE>(resource_path, std::forward<ARGS>(args)...);
            resource.get()->reload(*this);
            _loaded_resources.push_back(static_cast<std::shared_ptr<Resource>>(resource));
            return kstd::Result {resource};
        }
    };
}// namespace aetherium