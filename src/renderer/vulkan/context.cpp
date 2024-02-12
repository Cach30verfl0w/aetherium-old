//  Copyright 2024 Cedric Hammes/Cach30verfl0w
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "aetherium/renderer/vulkan/context.hpp"
#include "SDL2/SDL_vulkan.h"
#include "kstd/safe_alloc.hpp"
#include "spdlog/spdlog.h"

namespace aetherium::renderer::vulkan {
    namespace {
        auto enumerate_available_layers() -> kstd::Result<std::vector<std::string>> {
            uint32_t count = 0;
            VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr), "Unable to enumerate available layers: {}")
            std::vector<VkLayerProperties> instance_layers {count};
            VK_CHECK(vkEnumerateInstanceLayerProperties(&count, instance_layers.data()),
                     "Unable to enumerate available layers: {}")

            std::vector<std::string> layer_names {};
            for(const auto& properties : instance_layers) {
                layer_names.emplace_back(properties.layerName);
            }
            return layer_names;
        }

        auto get_device_local_heap(VkPhysicalDevice device_handle) -> uint32_t {
            VkPhysicalDeviceMemoryProperties memory_properties {};
            vkGetPhysicalDeviceMemoryProperties(device_handle, &memory_properties);

            uint32_t local_heap_size = 0;
            for(uint32_t i = 0; i < memory_properties.memoryHeapCount; i++) {
                const auto heap = memory_properties.memoryHeaps[i];
                if((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    continue;

                local_heap_size += heap.size;
            }
            return local_heap_size;
        }

        constexpr auto compare_by_local_heap = [](const auto& left, const auto& right) -> bool {
            return get_device_local_heap(left) < get_device_local_heap(right);
        };
    }// namespace

    VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                         VkDebugUtilsMessageTypeFlagsEXT types,
                                                         const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                         void* user_data) {
        printf("%s\n", callback_data->pMessage);
        // SPDLOG_DEBUG("Vulkan -> {}", callback_data->pMessage);
        UNUSED_PARAMETER(user_data);
        return VK_FALSE;
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
    VulkanContext::VulkanContext(Window& window, const char* name, uint8_t major, uint8_t minor, uint8_t patch) :
            _window {&window} {
        using namespace std::string_literals;

        VK_CHECK_EX(volkInitialize(), "Unable to create Vulkan context: {}")
        // TODO: Only use validation layer if debug build and validate existence of layer

        const std::vector<const char*> enabled_layers = {
#ifdef BUILD_DEBUG
                "VK_LAYER_KHRONOS_validation"
#endif
        };

        const auto available_layers = enumerate_available_layers();
        for(const auto& layer_name : enabled_layers) {
            std::string layer_name_str {layer_name};
            if(std::find(available_layers->cbegin(), available_layers->cend(), layer_name_str) ==
               available_layers->cend()) {
                throw std::runtime_error {
                        fmt::format("Unable to create vulkan context: Layer '{}' not available", layer_name_str)};
            }
        }

        // Get SDL window extensions
        uint32_t window_ext_count = 0;
        if(!SDL_Vulkan_GetInstanceExtensions(window.get_window_handle(), &window_ext_count, nullptr)) {
            throw std::runtime_error {"Unable to create vulkan context: Unable to get instance extension count"s};
        }

        auto extensions = std::vector<const char*> {window_ext_count};
        if(!SDL_Vulkan_GetInstanceExtensions(window.get_window_handle(), &window_ext_count, extensions.data())) {
            throw std::runtime_error {"Unable to create vulkan context: Unable to get instance extension names"s};
        }

#ifdef BUILD_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        extensions.push_back("VK_KHR_get_surface_capabilities2");
        SPDLOG_DEBUG("Initializing Vulkan Context with {} extension(s) and {} layer(s)", extensions.size(),
                     enabled_layers.size());

        // Create application
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
        instance_create_info.enabledExtensionCount = extensions.size();
        instance_create_info.ppEnabledExtensionNames = extensions.data();
        instance_create_info.enabledLayerCount = enabled_layers.size();
        instance_create_info.ppEnabledLayerNames = enabled_layers.data();
        VK_CHECK_EX(vkCreateInstance(&instance_create_info, nullptr, &_instance), "Unable to create Vulkan context: {}")
        volkLoadInstance(_instance);

        // Create surface
        if(!SDL_Vulkan_CreateSurface(window.get_window_handle(), _instance, &_surface)) {
            throw std::runtime_error {fmt::format("Unable to create vulkan context: {}", SDL_GetError())};
        }

        // Create debug utils messenger
#ifdef BUILD_DEBUG
        SPDLOG_DEBUG("Initializing Vulkan debug utils for debug message handling");
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info {};
        debug_utils_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_utils_create_info.pfnUserCallback = vulkan_debug_callback;

        VK_CHECK_EX(
                vkCreateDebugUtilsMessengerEXT(_instance, &debug_utils_create_info, nullptr, &_debug_utils_messenger),
                "Unable to create Vulkan context: {}")
#endif
    }

    VulkanContext::VulkanContext(VulkanContext&& other) noexcept :// NOLINT
            _instance {other._instance},
            _window {other._window} {
        other._instance = nullptr;
        other._window = nullptr;
#ifdef BUILD_DEBUG
        _debug_utils_messenger = other._debug_utils_messenger;
        other._debug_utils_messenger = nullptr;
#endif
    }

    VulkanContext::~VulkanContext() noexcept {
        if(_surface != nullptr) {
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = nullptr;
        }

#ifdef BUILD_DEBUG
        if(_debug_utils_messenger != nullptr) {
            vkDestroyDebugUtilsMessengerEXT(_instance, _debug_utils_messenger, nullptr);
            _debug_utils_messenger = nullptr;
        }
#endif

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
    auto VulkanContext::find_device(DeviceSearchStrategy strategy, bool only_dedicated) const noexcept
            -> kstd::Result<VulkanDevice> {
        using namespace std::string_literals;

        uint32_t device_count = 0;
        VK_CHECK(vkEnumeratePhysicalDevices(_instance, &device_count, nullptr), "Unable to create device: {}")
        std::vector<VkPhysicalDevice> devices {device_count};
        VK_CHECK(vkEnumeratePhysicalDevices(_instance, &device_count, devices.data()), "Unable to create device: {}")
        std::sort(devices.begin(), devices.end(), compare_by_local_heap);

        // Get physical device and create
        VkPhysicalDevice physical_device;
        switch(strategy) {
            case DeviceSearchStrategy::HIGHEST_PERFORMANCE: physical_device = devices.at(0); break;
            case DeviceSearchStrategy::LOWEST_PERFORMANCE: physical_device = devices.at(devices.size() - 1);
        }
        return kstd::try_construct<VulkanDevice>(physical_device);
    }

    auto VulkanContext::operator=(VulkanContext&& other) noexcept -> VulkanContext& {
        _instance = other._instance;
        other._instance = nullptr;
        _window = other._window;
        other._window = nullptr;
#ifdef BUILD_DEBUG
        _debug_utils_messenger = other._debug_utils_messenger;
        other._debug_utils_messenger = nullptr;
#endif
        return *this;
    }

    auto VulkanContext::get_surface_properties(const VulkanDevice& device) const noexcept
            -> kstd::Result<VkSurfaceCapabilities2KHR> {
        VkPhysicalDeviceSurfaceInfo2KHR surface_info {};
        surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
        surface_info.surface = _surface;

        VkSurfaceCapabilities2KHR surface_capabilities {};
        surface_capabilities.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilities2KHR(device.get_physical_device(), &surface_info,
                                                            &surface_capabilities),
                 "Unable to get surface properties: {}")
        return surface_capabilities;
    }

    auto VulkanContext::get_window() const noexcept -> Window* {
        return _window;
    }

    auto VulkanContext::operator*() const noexcept -> VkInstance {
        return _instance;
    }
}// namespace aetherium::renderer::vulkan
