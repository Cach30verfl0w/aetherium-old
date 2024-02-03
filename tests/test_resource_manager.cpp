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

#include <aetherium/renderer/shader.hpp>
#include <gtest/gtest.h>
#include <utility>

using namespace aetherium;

class TestResource final : public Resource {
    std::string _text;

    public:
    explicit TestResource(fs::path path, const kstd::reflect::RTTI* runtime_type) ://NOLINT
            Resource {std::move(path), runtime_type} {
    }
    ~TestResource() noexcept final = default;
    KSTD_DEFAULT_MOVE(TestResource, TestResource);
    KSTD_NO_COPY(TestResource, TestResource);

    kstd::Result<void> reload(const aetherium::ResourceManager& resource_manager) noexcept final {
        std::ifstream stream {_resource_path};
        _text = std::string {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
        return {};
    }

    [[nodiscard]] auto get_text() const noexcept -> const std::string& {
        return _text;
    }
};

TEST(aetherium_ResourceManager, test_load_resource) {
    spdlog::set_level(spdlog::level::debug);

    ResourceManager resource_manager {".."};
    auto resource = resource_manager.load_resource<TestResource>("test", "resource.txt");
    resource.throw_if_error();
    ASSERT_EQ(resource->get_text(), "This is a test text");
}

TEST(aetherium_ResourceManager, test_get_resource) {
    spdlog::set_level(spdlog::level::debug);

    ResourceManager resource_manager {".."};
    resource_manager.load_resource<TestResource>("test", "resource.txt").throw_if_error();
    ASSERT_EQ(resource_manager.get_resource<TestResource>("test", "resource.txt")->get_text(),
              "This is a test text");
}

TEST(aetherium_ResourceManager, test_get_or_load_resource) {
    spdlog::set_level(spdlog::level::debug);

    ResourceManager resource_manager {".."};
    ASSERT_EQ(resource_manager.get_or_load<TestResource>("test", "resource.txt")->get_text(),
              "This is a test text");
}

TEST(aetherium_ResourceManager, test_reload_resources) {
    spdlog::set_level(spdlog::level::debug);

    ResourceManager resource_manager {".."};
    resource_manager.load_resource<TestResource>("test", "resource.txt").throw_if_error();
    resource_manager.reload_by_type<TestResource>().throw_if_error();
    resource_manager.reload().throw_if_error();
}

TEST(aetherium_ResourceManager, test_load_shader) {
    using namespace renderer;
    spdlog::set_level(spdlog::level::debug);

    ResourceManager resource_manager {".."};
    resource_manager.load_resource<Shader>("test", "shader.glsl", ShaderKind::VERTEX).throw_if_error();
}
