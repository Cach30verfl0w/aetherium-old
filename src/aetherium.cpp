// Copyright 2024 Karma Krafts & associates
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

#include "aetherium/aetherium.hpp"
#include <kstd/streams/stream.hpp>
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace aetherium {
    namespace {
        auto get_physical_devices(const VkInstance& instance) noexcept -> kstd::Result<std::vector<VkPhysicalDevice>> {
            uint32_t device_count = 0;
            VK_CHECK(vkEnumeratePhysicalDevices(instance, &device_count, nullptr), "Unable to list devices: {}");
            std::vector<VkPhysicalDevice> devices {device_count};
            VK_CHECK(vkEnumeratePhysicalDevices(instance, &device_count, devices.data()), "Unable to list devices: {}");
            return devices;
        }

        auto get_device_heap_size(const VkPhysicalDevice& device) noexcept -> kstd::Result<uint32_t> {
            VkPhysicalDeviceMemoryProperties properties {};
            vkGetPhysicalDeviceMemoryProperties(device, &properties);

            uint32_t heap_size = 0;
            for (int i = 0; i < properties.memoryHeapCount; i++) {
                const auto memory_heap = properties.memoryHeaps[i];
                if ((memory_heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) == VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    heap_size += memory_heap.size;
                }
            }
            return heap_size;
        }

        constexpr auto biggest_heap_comperator = [](const auto& lhs, const auto& rhs) noexcept -> bool {
            return get_device_heap_size(lhs) < get_device_heap_size(rhs);
        };
    }

    Device::~Device() noexcept {
        if (_vk_virtual_device == nullptr) {
            return;
        }

        vkDestroyDevice(_vk_virtual_device, nullptr);
        _vk_virtual_device = nullptr;
    }

    auto Device::initialize(VkPhysicalDevice device) noexcept -> kstd::Result<void> {
        _vk_physical_device = device;
        vkGetPhysicalDeviceProperties(_vk_physical_device, &_device_properties);
        constexpr std::array queue_priorities = {
            1.0f
        };

        VkDeviceQueueCreateInfo device_queue_create_info {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.pQueuePriorities = queue_priorities.data();
        device_queue_create_info.queueFamilyIndex = 0;
        device_queue_create_info.queueCount = 1;

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &device_queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.enabledExtensionCount = 0;
        device_create_info.enabledLayerCount = 0;
        VK_CHECK(vkCreateDevice(_vk_physical_device, &device_create_info, nullptr, &_vk_virtual_device), "Unable to init device: {}");
        SPDLOG_DEBUG("Successfully created virtual device");
        return {};
    }

    auto Device::get_name() const noexcept -> std::string {
        return std::string {_device_properties.deviceName};
    }

    Application::Application(const std::string& name, const uint32_t version) {
        spdlog::set_pattern(fmt::format("[%Y-%m-%d %H:%M:%S:%e] [%s:%# %l/%P] ({}): %v", name));
#if BUILD_DEBUG
        spdlog::set_level(spdlog::level::debug);
        constexpr std::array validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };
#else
        constexpr std::array<const char*, 0> validation_layers {};
#endif

        // Create instance information and instance itself
        VkApplicationInfo application_info {};
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.apiVersion = VK_API_VERSION_1_3; // Newest vulkan version
        application_info.pEngineName = "Aetherium Engine";
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.pApplicationName = name.data();
        application_info.applicationVersion = version;
        application_info.pNext = nullptr;

        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledExtensionCount = 0;
        instance_create_info.enabledLayerCount = validation_layers.size();
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
        instance_create_info.pNext = nullptr;
        VK_CHECK_EX(vkCreateInstance(&instance_create_info, nullptr, &_vulkan_instance), "Unable to init app: {}");
        SPDLOG_DEBUG("Successfully create Vulkan instance");

        // Find best physical device and initialize
        const auto physical_devices = get_physical_devices(_vulkan_instance).get_or_throw();
        if (physical_devices.empty()) {
            throw std::runtime_error("Unable to init app: Unable to find physical devices for renderer");
        }
        SPDLOG_DEBUG("Found {} physical devices", physical_devices.size());
        _vulkan_device.initialize(kstd::streams::stream(physical_devices)
            .sort(biggest_heap_comperator)
            .find_any().get());
        SPDLOG_INFO("Successfully initialized device '{}' for Vulkan API", _vulkan_device.get_name());
    }

    Application::~Application() noexcept {
        _vulkan_device.~Device();
        vkDestroyInstance(_vulkan_instance, nullptr);
    }


    auto vk_error_message(const VkResult result) noexcept -> std::string {
         switch(result) {
             case VK_SUCCESS: return "Succeded";
             case VK_ERROR_DEVICE_LOST: return "Device lost";
             case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of host memory";
             case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of device memory";
             case VK_ERROR_INCOMPATIBLE_DRIVER: return "Incompatible driver";
             case VK_ERROR_MEMORY_MAP_FAILED: return "Memory map failed";
             case VK_ERROR_INITIALIZATION_FAILED: return "Initialization failed";
             default: return "Unknown/Unidentified";
         }
    }
}