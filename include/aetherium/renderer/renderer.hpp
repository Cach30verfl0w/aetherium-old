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
#include <fmt/format.h>
#include <kstd/result.hpp>
#include <kstd/safe_alloc.hpp>
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vulkan/vulkan_core.h>

#define VK_CHECK_EX(x, m)                                                                                              \
    if(const auto result = (x); result != VK_SUCCESS) {                                                                \
        throw std::runtime_error {fmt::format((m), get_vk_error_message(result))};                                     \
    }

#define VK_CHECK(x, m)                                                                                                 \
    if(const auto result = (x); result != VK_SUCCESS) {                                                                \
        return kstd::Error {fmt::format((m), get_vk_error_message(result))};                                           \
    }

namespace aetherium::renderer {
    enum DeviceSearchStrategy {
        HIGHEST_PERFORMANCE,
        LOWEST_PERFORMANCE
    };

    class VulkanDevice {
        VkPhysicalDevice _physical_device {};
        VkDevice _virtual_device {};
        VkPhysicalDeviceProperties _properties {};

        public:
        explicit VulkanDevice(VkPhysicalDevice device);
        VulkanDevice(VulkanDevice&& device) noexcept;
        ~VulkanDevice() noexcept;
        KSTD_NO_COPY(VulkanDevice, VulkanDevice);

        [[nodiscard]] auto get_name() const noexcept -> std::string;

        auto operator=(VulkanDevice&& other) noexcept -> VulkanDevice&;
    };

    class VulkanContext {
        VkInstance _vk_instance {};
#ifdef BUILD_DEBUG
        VkDebugUtilsMessengerEXT _debug_utils_messenger {};
#endif

        public:
        VulkanContext(std::string_view name, uint32_t version);
        VulkanContext(VulkanContext&& device) noexcept;
        ~VulkanContext() noexcept;
        KSTD_NO_COPY(VulkanContext, VulkanContext);

        [[nodiscard]] auto find_device(DeviceSearchStrategy strategy) const noexcept -> kstd::Result<VulkanDevice>;

        auto operator=(VulkanContext&& other) noexcept -> VulkanContext&;
    };

    [[nodiscard]] auto get_vk_error_message(VkResult result) noexcept -> std::string;
}// namespace aetherium::renderer