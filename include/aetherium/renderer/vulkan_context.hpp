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

#include "aetherium/renderer/vulkan_device.hpp"
#include "aetherium/window.hpp"
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <kstd/result.hpp>

namespace aetherium::renderer {
    /**
     * This enum identifies the strategy of the device search. This makes possible to search the device with the highest
     * performance or the lowest performance. The performance of the devices is identified by the device-local heap
     * size.
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    enum DeviceSearchStrategy {
        /**
         * This strategy creates the device with the highest device-local heap size
         */
        HIGHEST_PERFORMANCE,
        /**
         * This strategy creates the device with the lowest device-local heap size
         */
        LOWEST_PERFORMANCE
    };

    /**
     * This class holds the context of the Vulkan renderer. This contains debug information if enabled and the instance
     * of the Vulkan application.
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    class VulkanContext final {
        VkInstance _instance {};
        VkSurfaceKHR _surface {};
        Window* _window {};

#ifdef BUILD_DEBUG
        VkDebugUtilsMessengerEXT _debug_utils_messenger {};
#endif

        public:
        friend class Swapchain;

        /**
         * This constructor creates the vulkan constructor by the specified application name and the major, minor and
         * patch version of the application. If the engine is built in debug mode, the context also instruments the
         * debug utils to read debug prints.
         *
         * @param name  The application's name
         * @param major The application's major version
         * @param minor The application's minor version
         * @param patch The application's patch version
         *
         * @author      Cedric Hammes
         * @since       04/02/2024
         */
        VulkanContext(Window& window, const char* name, uint8_t major, uint8_t minor, uint8_t patch);
        VulkanContext(VulkanContext&& other) noexcept;
        ~VulkanContext() noexcept;
        KSTD_NO_COPY(VulkanContext, VulkanContext);

        /**
         * This function enumerates all available physical device handles and sorted them by the size of the
         * device-local heap. Then (based on the search type strategy) the device with the lowest heap size
         * or the device with the highest heap size gets returned.
         *
         * When the `only_dedicated` flag is set, the function sorts all non-dedicated graphics device out
         * of the list of available devices.
         *
         * @param strategy       The device priority strategy
         * @param only_dedicated Weather filtering all non-dedicated devices out
         * @return               The device itself or an error
         *
         * @author               Cedric Hammes
         * @since                04/02/2024
         */
        [[nodiscard]] auto find_device(DeviceSearchStrategy strategy, bool only_dedicated = false) const noexcept
                -> kstd::Result<VulkanDevice>;
        [[nodiscard]] auto get_window() noexcept -> Window*;

        auto operator=(VulkanContext&& other) noexcept -> VulkanContext&;
    };
}// namespace aetherium::renderer
