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

#include "aetherium/renderer/renderer.hpp"

namespace aetherium::renderer {
    namespace {
        [[nodiscard]] auto get_device_local_heap(VkPhysicalDevice physical_device) noexcept -> uint32_t {
            VkPhysicalDeviceMemoryProperties properties {};
            vkGetPhysicalDeviceMemoryProperties(physical_device, &properties);

            uint32_t heap_size = 0;
            for (uint32_t i = 0; i < properties.memoryHeapCount; i++) {
                const auto memory_heap = properties.memoryHeaps[i];
                if ((memory_heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    continue;
                heap_size += memory_heap.size;
            }
            return heap_size;
        }

        constexpr auto device_heap_comparator = [](const auto& left, const auto& right) -> bool {
            return get_device_local_heap(left) < get_device_local_heap(right);
        };
    }

    VulkanDevice::VulkanDevice(const VkPhysicalDevice physical_device) :// NOLINT
            _physical_device {physical_device} {
        vkGetPhysicalDeviceProperties(physical_device, &_properties);
        constexpr std::array properties {1.0f};

        VkDeviceQueueCreateInfo device_queue_create_info {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.pQueuePriorities = properties.data();
        device_queue_create_info.queueFamilyIndex = 0;
        device_queue_create_info.queueCount = 1;

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.enabledLayerCount = 0;
        device_create_info.enabledExtensionCount = 0;
        device_create_info.pQueueCreateInfos = &device_queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        VK_CHECK_EX(vkCreateDevice(physical_device, &device_create_info, nullptr, &_virtual_device),
                    "Unable to create device {}")
    }

    VulkanDevice::VulkanDevice(VulkanDevice&& device) noexcept :// NOLINT
            _physical_device{device._physical_device},
            _virtual_device{device._virtual_device},
            _properties{device._properties}
    {
        device._virtual_device = nullptr;
    }

    VulkanDevice::~VulkanDevice() noexcept {
        if (_virtual_device == nullptr) {
            return;
        }

        vkDestroyDevice(_virtual_device, nullptr);
        _virtual_device = nullptr;
    }

    auto VulkanDevice::get_name() const noexcept -> std::string {
        return std::string{_properties.deviceName};
    }

    auto VulkanDevice::operator=(VulkanDevice&& other) noexcept -> VulkanDevice& {
        _physical_device = other._physical_device;
        _virtual_device = other._virtual_device;
        _properties = other._properties;
        other._virtual_device = nullptr;
        return *this;
    }

    VulkanContext::VulkanContext(const std::string_view name, const uint32_t version) {
        VkApplicationInfo application_info {};
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pApplicationName = name.data();
        application_info.applicationVersion = version;
        application_info.pEngineName = "Aetherium Engine";
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.apiVersion = VK_API_VERSION_1_3;

        std::vector layers = {
                "VK_LAYER_KHRONOS_validation"
        };
        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledExtensionCount = 0;
        instance_create_info.enabledLayerCount = layers.size();
        instance_create_info.ppEnabledLayerNames = layers.data();
        VK_CHECK_EX(vkCreateInstance(&instance_create_info, nullptr, &_vk_instance), "Unable to create app: {}")
    }

    VulkanContext::VulkanContext(aetherium::renderer::VulkanContext&& device) noexcept : //NOLINT
            _vk_instance{device._vk_instance}
    {
        device._vk_instance = nullptr;
    }

    VulkanContext::~VulkanContext() noexcept {
        if (_vk_instance == nullptr) {
            return;
        }

        vkDestroyInstance(_vk_instance, nullptr);
        _vk_instance = nullptr;
    }

    auto VulkanContext::find_device(const aetherium::renderer::DeviceSearchStrategy strategy) const noexcept -> kstd::Result<VulkanDevice> {
        uint32_t device_count = 0;
        VK_CHECK(vkEnumeratePhysicalDevices(_vk_instance, &device_count, nullptr), "Unable to find device: {}")
        std::vector<VkPhysicalDevice> devices {device_count};
        VK_CHECK(vkEnumeratePhysicalDevices(_vk_instance, &device_count, devices.data()), "Unable to find device: {}")
        // clang-format off
        const auto sorted_devices = kstd::streams::stream(devices)
            .sort(device_heap_comparator)
            .collect<std::vector>(kstd::streams::collectors::push_back);
        // clang-format on

        // Get physical device and create
        VkPhysicalDevice physical_device;
        switch (strategy) {
            case DeviceSearchStrategy::HIGHEST_PERFORMANCE: physical_device = sorted_devices.at(0); break;
            case DeviceSearchStrategy::LOWEST_PERFORMANCE: physical_device = sorted_devices.at(sorted_devices.size() - 1);
        }
        return kstd::try_construct<VulkanDevice>(physical_device);
    }

    auto VulkanContext::operator=(aetherium::renderer::VulkanContext&& other) noexcept -> VulkanContext& {
        _vk_instance = other._vk_instance;
        other._vk_instance = nullptr;

        return *this;
    }

    [[nodiscard]] auto get_vk_error_message(const VkResult result) noexcept -> std::string {
        switch (result) {
            case VK_SUCCESS: return "Succeeded";
            case VK_ERROR_DEVICE_LOST: return "Device lost";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of device memory";
            case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of host memory";
            default: return "Unknown";
        }
    }
}