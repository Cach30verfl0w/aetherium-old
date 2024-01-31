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

#include <gtest/gtest.h>
#include <aetherium/resource.hpp>

using namespace aetherium;

TEST(aetherium_ResourceManager, test_resource_manager) {
    spdlog::set_level(spdlog::level::debug);

    enum ResourceType {
        MAIN
    };

    ResourceManager<ResourceType> resource_manager {};
    resource_manager.add_directory(ResourceType::MAIN, "../assets/test").throw_if_error();
    const std::string text = resource_manager.load_resource(ResourceType::MAIN, "resource.txt", read_string_factory)
                              .get_or_throw();
    ASSERT_EQ(text, "This is a test text");
}