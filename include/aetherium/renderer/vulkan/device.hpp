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
#include "aetherium/utils.hpp"
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <kstd/result.hpp>
#include <kstd/safe_alloc.hpp>
#include <type_traits>

namespace aetherium::renderer::vulkan {
    /**
     * This class is a wrapper around the command buffer to perform actions and push them to the queue.
     *
     * @author Cedric Hammes
     * @since  06/02/2024
     */
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
        VkQueue _graphics_queue;// TODO: Support multiple queues

        public:
        /**
         * This constructor creates an empty vulkan device
         *
         * @author Cedric Hammes
         * @since  04/02/2024
         */
        VulkanDevice() noexcept;

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
         * This function creates a one-time command buffer and executes the specified function. After the run, the
         * command buffer get submitted into the queue and the program waits for the execution.
         *
         * @tparam F       The function type
         * @param function The function itself
         * @return         Nothing or an error
         *
         * @author         Cedric Hammes
         * @since          06/02/2024
         */
        template<typename F>
        [[maybe_unused]] [[nodiscard]] auto emit_command_buffer(F&& function) const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto get_physical_device() const noexcept -> VkPhysicalDevice;
        [[nodiscard]] auto get_virtual_device() const noexcept -> VkDevice;
        [[nodiscard]] auto get_graphics_queue() const noexcept -> VkQueue;

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
     * @since  06/02/2024
     */
    class CommandPool;

    /**
     * This class is a wrapper around the command buffer to perform actions and push them to the queue.
     *
     * @author Cedric Hammes
     * @since  06/02/2024
     */
    class CommandBuffer final {
        const CommandPool* _command_pool;
        VkCommandBuffer _command_buffer;

        public:
        /**
         * This constructor creates an empty command buffer
         *
         * @author Cedric Hammes
         * @since  06/02/2024
         */
        CommandBuffer() noexcept;

        /**
         * This constructor creates a command buffer with the specified command buffer and the command pool.
         *
         * @param command_pool   The command pool with which the buffer was allocated
         * @param command_buffer The command buffer itself
         *
         * @author               Cedric Hammes
         * @since                06/02/2024
         */
        CommandBuffer(const CommandPool* command_pool, VkCommandBuffer command_buffer);
        ~CommandBuffer() noexcept;
        CommandBuffer(CommandBuffer&& other) noexcept;
        KSTD_NO_COPY(CommandBuffer, CommandBuffer);

        [[nodiscard]] auto begin(VkCommandBufferUsageFlags usage = 0) const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto end() const noexcept -> kstd::Result<void>;

        auto operator=(CommandBuffer&& other) noexcept -> CommandBuffer&;
        auto operator*() const noexcept -> VkCommandBuffer;
    };

    /**
     * This class is a wrapper around the command pool. The command pool allows the developer to allocate command buffer
     * to perform actions to the queue.
     *
     * @author Cedric Hammes
     * @since  06/02/2024
     */
    class CommandPool final {
        const VulkanDevice* _vulkan_device;
        VkCommandPool _command_pool;

        public:
        friend class VulkanRenderer;
        friend class CommandBuffer;

        /**
         * This constructor creates an empty command pool
         *
         * @author Cedric Hammes
         * @since  06/02/2024
         */
        CommandPool() noexcept;

        /**
         * This constructor creates a command pool by the specified device
         *
         * @param vulkan_device The device for the pool
         *
         * @author              Cedric Hammes
         * @since               06/02/2024
         */
        explicit CommandPool(const VulkanDevice* vulkan_device);
        ~CommandPool() noexcept;
        CommandPool(CommandPool&& other) noexcept;
        KSTD_NO_COPY(CommandPool, CommandPool);

        /**
         * This function allocates the specified count of wrapped command buffers.
         *
         * @param count The count of newly allocated command buffers
         * @return      The command buffers or an error
         *
         * @author      Cedric Hammes
         * @since       06/02/2024
         */
        [[nodiscard]] auto allocate_command_buffers(uint32_t count) const noexcept
                -> kstd::Result<std::vector<CommandBuffer>>;

        auto operator=(CommandPool&& other) noexcept -> CommandPool&;
        auto operator*() const noexcept -> VkCommandPool;
    };
}// namespace aetherium::renderer::vulkan
