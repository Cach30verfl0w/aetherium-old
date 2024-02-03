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

#include "aetherium/window.hpp"
#include "aetherium/utils.hpp"
#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <kstd/result.hpp>
#include <kstd/safe_alloc.hpp>
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vulkan/vulkan_core.h>

namespace aetherium::renderer {
    class VulkanContext;

    /**
     * This enum identifies the strategy of the device acquire function. The user can acquire the least or most
     * performant device.
     *
     * @since  01/02/2024
     * @author Cedric Hammes
     */
    enum DeviceSearchStrategy {
        HIGHEST_PERFORMANCE,
        LOWEST_PERFORMANCE
    };

    /**
     * This object holds the handle of the Vulkan device, the physical device handle and the device properties. This
     * object allows the developer to operate with the device and it's properties itself.
     *
     * @since  01/02/2024
     * @author Cedric Hammes
     */
    class VulkanDevice {
        VkPhysicalDevice _physical_device {};
        VkDevice _virtual_device {};
        VkPhysicalDeviceProperties _properties {};
        const VulkanContext* _vulkan_context;

        public:
        /**
         * This constructor creates the vulkan device by the specified physical device handle.
         *
         * @param device The physical device handle of the device
         * @since        01/02/2024
         * @author       Cedric Hammes
         */
        explicit VulkanDevice(const VulkanContext* vulkan_context, VkPhysicalDevice device);

        VulkanDevice(VulkanDevice&& device) noexcept;
        ~VulkanDevice() noexcept;
        KSTD_NO_COPY(VulkanDevice, VulkanDevice);

        /**
         * This function returns the literal name of the device
         *
         * @return The name of the device
         * @since  01/02/2024
         * @author Cedric Hammes
         */
        [[nodiscard]] auto get_name() const noexcept -> std::string;

        /**
         * This function enumerates all heap types, which are available on this graphics card.
         *
         * @param type_filter The type filter
         * @param properties  The memory properties
         * @return            The memory type index
         * @since             01/02/2024
         * @author            Cedric Hammes
         */
        [[nodiscard]] auto get_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags properties) const noexcept
                -> kstd::Option<uint32_t>;

        auto operator=(VulkanDevice&& other) noexcept -> VulkanDevice&;
    };

    /**
     * This object holds the Vulkan instance of the application and the debug utils messenger if the engine it built for
     * debug. The context can be used to acquire devices and interact with the Vulkan API.
     *
     * @since  01/02/2024
     * @author Cedric Hammes
     */
    class VulkanContext {
        VkInstance _vk_instance {};
#ifdef BUILD_DEBUG
        VkDebugUtilsMessengerEXT _debug_utils_messenger {};
#endif

        public:
        /**
         * This constructor creates the vulkan instance with the name and version of the application. These information
         * are given to the Vulkan API.
         *
         * @param name    The name of the application
         * @param version The version of the application
         * @since         01/02/2024
         * @author        Cedric Hammes
         */
        VulkanContext(const Window& window, std::string_view name, uint32_t version);

        VulkanContext(VulkanContext&& device) noexcept;
        ~VulkanContext() noexcept;
        KSTD_NO_COPY(VulkanContext, VulkanContext);

        /**
         * This function enumerates all available devices and sort them by the size of the device-local heap. Based on
         * the strategy, this function returns the device with the highest heap or the lowest heap.
         *
         * @param strategy The strategy to find the device
         * @return         The device with the highest or lowest heap
         * @since          01/02/2024
         * @author         Cedric Hammes
         */
        [[nodiscard]] auto find_device(DeviceSearchStrategy strategy) const noexcept -> kstd::Result<VulkanDevice>;

        auto operator=(VulkanContext&& other) noexcept -> VulkanContext&;
    };

    [[nodiscard]] auto get_vk_error_message(VkResult result) noexcept -> std::string;
}// namespace aetherium::renderer