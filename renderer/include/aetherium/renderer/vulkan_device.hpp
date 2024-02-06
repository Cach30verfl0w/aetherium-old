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
#include <kstd/streams/collectors.hpp>
#include <kstd/streams/stream.hpp>
#include <kstd/safe_alloc.hpp>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <kstd/result.hpp>
#include <type_traits>

namespace aetherium::renderer {
    class CommandBuffer;

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
        friend class CommandPool;
        friend class CommandBuffer;

        /**
         * This constructor creates the vulkan device by the specified physical device.
         *
         * @param physical_device The handle to the physical device
         *
         * @author Cedric Hammes
         * @since  04/02/2024
         */
        VulkanDevice() noexcept;
        explicit VulkanDevice(VkPhysicalDevice physical_device);
        VulkanDevice(VulkanDevice&& other) noexcept;
        ~VulkanDevice() noexcept;
        KSTD_NO_COPY(VulkanDevice, VulkanDevice);

        template<typename F>
        [[maybe_unused]] [[nodiscard]] auto emit_command_buffer(F&& function) const noexcept -> kstd::Result<void>;

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

    /**
     * This class is a wrapper around the command pool. The command pool allows the developer to allocate command buffer
     * to perform actions to the queue.
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    class CommandPool;

    class CommandBuffer final {
        const CommandPool* _command_pool;
        VkCommandBuffer _command_buffer;

        public:
        friend class VulkanDevice;

        CommandBuffer(const CommandPool* command_pool, VkCommandBuffer command_buffer);
        ~CommandBuffer() noexcept;
        CommandBuffer(CommandBuffer&& other) noexcept;
        KSTD_NO_COPY(CommandBuffer, CommandBuffer);

        auto operator=(CommandBuffer&& other) noexcept -> CommandBuffer&;
    };

    /**
     * This class is a wrapper around the command pool. The command pool allows the developer to allocate command buffer
     * to perform actions to the queue.
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    class CommandPool final {
        const VulkanDevice* _vulkan_device;
        VkCommandPool _command_pool;

        public:
        friend class CommandBuffer;

        explicit CommandPool(const VulkanDevice* vulkan_device);
        ~CommandPool() noexcept;
        CommandPool(CommandPool&& other) noexcept;
        KSTD_NO_COPY(CommandPool, CommandPool);

        [[nodiscard]] auto allocate_command_buffers(uint32_t count) const noexcept
                -> kstd::Result<std::vector<CommandBuffer>>;

        auto operator=(CommandPool&& other) noexcept -> CommandPool&;
    };
}// namespace aetherium::renderer
