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
#include <kstd/defaults.hpp>
#include <vulkan/vulkan_core.h>
#include <fmt/format.h>
#include <string>

#define VK_CHECK(o, m) if (const auto result = (o); result != VK_SUCCESS) {                                            \
        throw std::runtime_error {fmt::format((m), vk_error_message(result))};                                         \
    }

namespace aetherium {
    class Application {
        VkInstance _vulkan_instance {};
        VkPhysicalDevice _vulkan_physical_device {};
        VkDevice _vulkan_logical_device {};

        public:
        Application(const std::string& name, uint32_t version);
        KSTD_NO_MOVE_COPY(Application, Application);
    };

    auto vk_error_message(VkResult result) noexcept -> std::string;
}
