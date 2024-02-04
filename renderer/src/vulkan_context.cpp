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

#include "aetherium/renderer/vulkan_context.hpp"
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <kstd/safe_alloc.hpp>

namespace aetherium::renderer {
    namespace {
        auto get_device_local_heap(VkPhysicalDevice device_handle) -> uint32_t {
            VkPhysicalDeviceMemoryProperties memory_properties {};
            vkGetPhysicalDeviceMemoryProperties(device_handle, &memory_properties);

            uint32_t local_heap_size = 0;
            for (uint32_t i = 0; i < memory_properties.memoryHeapCount; i++) {
                const auto heap = memory_properties.memoryHeaps[i];
                if ((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    continue;

                local_heap_size += heap.size;
            }
            return local_heap_size;
        }

        constexpr auto compare_by_local_heap = [](const auto& left, const auto& right) -> bool {
            return get_device_local_heap(left) < get_device_local_heap(right);
        };
    }

    /**
     * This constructor creates the vulkan constructor by the specified application name and the major, minor and patch
     * version of the application. If the engine is built in debug mode, the context also instruments the debug utils to
     * read debug prints.
     *
     * @param name  The application's name
     * @param major The application's major version
     * @param minor The application's minor version
     * @param patch The application's patch version
     *
     * @author      Cedric Hammes
     * @since       04/02/2024
     */
    VulkanContext::VulkanContext(const char* name, uint8_t major, uint8_t minor, uint8_t patch) {
        VK_CHECK_EX(volkInitialize(), "Unable to create Vulkan context: {}")

        VkApplicationInfo application_info = {};
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.apiVersion = VK_API_VERSION_1_3;
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.pEngineName = "Aetherium";
        application_info.pApplicationName = name;
        application_info.applicationVersion = VK_MAKE_VERSION(major, minor, patch);

        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledExtensionCount = 0;
        instance_create_info.enabledLayerCount = 0;
        VK_CHECK_EX(vkCreateInstance(&instance_create_info, nullptr, &_instance), "Unable to create Vulkan context: {}")
        volkLoadInstance(_instance);
    }

    VulkanContext::VulkanContext(aetherium::renderer::VulkanContext&& other) noexcept :// NOLINT
            _instance {other._instance} {
        other._instance = nullptr;
    }

    VulkanContext::~VulkanContext() noexcept {
        if(_instance != nullptr) {
            vkDestroyInstance(_instance, nullptr);
            _instance = nullptr;
        }
    }

    /**
     * This function enumerates all available physical device handles and sorted them by the size of the device-local
     * heap. Then (based on the search type strategy) the device with the lowest heap size or the device with the
     * highest heap size gets returned.
     *
     * When the `only_dedicated` flag is set, the function sorts all non-dedicated graphics device out of the list of
     * available devices.
     *
     * @param strategy       The device priority strategy
     * @param only_dedicated Weather filtering all non-dedicated devices out
     * @return               The device itself or an error
     *
     * @author               Cedric Hammes
     * @since                04/02/2024
     */
    auto VulkanContext::find_device(aetherium::renderer::DeviceSearchStrategy strategy, bool only_dedicated) noexcept
            -> kstd::Result<VulkanDevice> {
        using namespace std::string_literals;

        uint32_t device_count = 0;
        VK_CHECK(vkEnumeratePhysicalDevices(_instance, &device_count, nullptr), "Unable to create device: {}")
        std::vector<VkPhysicalDevice> devices {device_count};
        VK_CHECK(vkEnumeratePhysicalDevices(_instance, &device_count, devices.data()), "Unable to create device: {}")

        // clang-format off
        const auto sorted_devices = kstd::streams::stream(devices)
            .sort(compare_by_local_heap)
            .filter([&](const VkPhysicalDevice& device) -> bool {
                if (only_dedicated) {
                    VkPhysicalDeviceProperties properties {};
                    vkGetPhysicalDeviceProperties(device, &properties);
                    return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
                }
                return true;
            })
            .collect<std::vector>(kstd::streams::collectors::push_back);
        // clang-format on

        // Get physical device and create
        VkPhysicalDevice physical_device;
        switch(strategy) {
            case DeviceSearchStrategy::HIGHEST_PERFORMANCE: physical_device = sorted_devices.at(0); break;
            case DeviceSearchStrategy::LOWEST_PERFORMANCE:
                physical_device = sorted_devices.at(sorted_devices.size() - 1);
        }
        return kstd::try_construct<VulkanDevice>(physical_device);
    }

    auto VulkanContext::operator=(aetherium::renderer::VulkanContext&& other) noexcept -> VulkanContext& {
        _instance = other._instance;
        other._instance = nullptr;
        return *this;
    }
}// namespace aetherium::renderer