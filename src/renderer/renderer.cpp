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
            for(uint32_t i = 0; i < properties.memoryHeapCount; i++) {
                const auto memory_heap = properties.memoryHeaps[i];
                if((memory_heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    continue;
                heap_size += memory_heap.size;
            }
            return heap_size;
        }

#ifdef BUILD_DEBUG
        VKAPI_ATTR auto VKAPI_CALL debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                        VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                        void* user_data) -> VkBool32 {
            // TODO: Debug Callback
            printf("%s\n", callback_data->pMessage);
            return VK_FALSE;
        }
#endif

        constexpr auto device_heap_comparator = [](const auto& left, const auto& right) -> bool {
            return get_device_local_heap(left) < get_device_local_heap(right);
        };

#if BUILD_DEBUG
        constexpr std::array validation_layers {"VK_LAYER_KHRONOS_validation"};

        constexpr std::array debug_extensions {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#else
        constexpr auto validation_layers = std::array<const char*, 0> {};
        constexpr auto debug_extensions = std::array<const char*, 0> {};
#endif
    }// namespace

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
            _physical_device {device._physical_device},
            _virtual_device {device._virtual_device},
            _properties {device._properties} {
        device._virtual_device = nullptr;
    }

    VulkanDevice::~VulkanDevice() noexcept {
        if(_virtual_device == nullptr) {
            return;
        }

        vkDestroyDevice(_virtual_device, nullptr);
        _virtual_device = nullptr;
    }

    auto VulkanDevice::get_name() const noexcept -> std::string {
        return std::string {_properties.deviceName};
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

        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledExtensionCount = debug_extensions.size();
        instance_create_info.ppEnabledExtensionNames = debug_extensions.data();
        instance_create_info.enabledLayerCount = validation_layers.size();
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
        VK_CHECK_EX(vkCreateInstance(&instance_create_info, nullptr, &_vk_instance), "Unable to create app: {}")

#if BUILD_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_info {};
        debug_utils_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_utils_info.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_utils_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        debug_utils_info.pfnUserCallback = debug_utils_callback;

        const auto create_debug_utils_messenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(_vk_instance, "vkCreateDebugUtilsMessengerEXT"));
        // clang-format off
        VK_CHECK_EX(
                create_debug_utils_messenger(_vk_instance, &debug_utils_info, nullptr, &_debug_utils_messenger),
                "Unable to create app: {}"
        )
        // clang-format on
#endif
    }

    VulkanContext::VulkanContext(aetherium::renderer::VulkanContext&& device) noexcept ://NOLINT
            _vk_instance {device._vk_instance} {
#ifdef BUILD_DEBUG
        _debug_utils_messenger = device._debug_utils_messenger;
        device._debug_utils_messenger = nullptr;
#endif
        device._vk_instance = nullptr;
    }

    VulkanContext::~VulkanContext() noexcept {
#ifdef BUILD_DEBUG
        if(_debug_utils_messenger != nullptr) {
            const auto destroy_debug_utils_messenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                    vkGetInstanceProcAddr(_vk_instance, "vkDestroyDebugUtilsMessengerEXT"));
            destroy_debug_utils_messenger(_vk_instance, _debug_utils_messenger, nullptr);
            _debug_utils_messenger = nullptr;
        }
#endif
        if(_vk_instance != nullptr) {
            vkDestroyInstance(_vk_instance, nullptr);
            _vk_instance = nullptr;
        }
    }

    auto VulkanContext::find_device(const aetherium::renderer::DeviceSearchStrategy strategy) const noexcept
            -> kstd::Result<VulkanDevice> {
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
        switch(strategy) {
            case DeviceSearchStrategy::HIGHEST_PERFORMANCE: physical_device = sorted_devices.at(0); break;
            case DeviceSearchStrategy::LOWEST_PERFORMANCE:
                physical_device = sorted_devices.at(sorted_devices.size() - 1);
        }
        return kstd::try_construct<VulkanDevice>(physical_device);
    }

    auto VulkanContext::operator=(aetherium::renderer::VulkanContext&& other) noexcept -> VulkanContext& {
        _vk_instance = other._vk_instance;
        other._vk_instance = nullptr;
#ifdef BUILD_DEBUG
        _debug_utils_messenger = other._debug_utils_messenger;
        other._debug_utils_messenger = nullptr;
#endif
        return *this;
    }

    [[nodiscard]] auto get_vk_error_message(const VkResult result) noexcept -> std::string {
        switch(result) {
            case VK_SUCCESS: return "Succeeded";
            case VK_ERROR_DEVICE_LOST: return "Device lost";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of device memory";
            case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of host memory";
            default: return "Unknown";
        }
    }
}// namespace aetherium::renderer