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
#include "aetherium/renderer/utils.hpp"
#include <aetherium/core/utils.hpp>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>

namespace aetherium::renderer {
    /**
     * This class is a wrapper around the Vulkan device handle and physical device handle.
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    class VulkanDevice final {
        VkPhysicalDevice _physical_device;
        VkDevice _virtual_device;
        VkPhysicalDeviceProperties _properties {};

        public:
        /**
         * This constructor creates the vulkan device by the specified physical device.
         *
         * @param physical_device The handle to the physical device
         *
         * @author Cedric Hammes
         * @since  04/02/2024
         */
        explicit VulkanDevice(VkPhysicalDevice physical_device);
        VulkanDevice(VulkanDevice&& other) noexcept;
        ~VulkanDevice() noexcept;
        KSTD_NO_COPY(VulkanDevice, VulkanDevice);

        /**
         * This function enumerates all available device queues and returns the first available device for the specific
         * task. TODO: Implement queue enumeration function
         *
         * @param queue_flags The flags of the wanted queue
         * @return            The queue or a none
         *
         * @author Cedric Hammes
         * @since  05/02/2024
         */
        [[maybe_unused]] [[nodiscard]] auto acquire_queue(VkQueueFlagBits queue_flags) const noexcept
                -> kstd::Option<VkQueue>;

        /**
         * This function returns the name of the device by the device properties.
         *
         * @return The name of the device
         *
         * @author Cedric Hammes
         * @since  04/02/2024
         */
        [[nodiscard]] inline auto get_name() const noexcept -> std::string {
            return std::string {_properties.deviceName};
        }

        auto operator=(VulkanDevice&& other) noexcept -> VulkanDevice&;
    };
}// namespace aetherium::renderer