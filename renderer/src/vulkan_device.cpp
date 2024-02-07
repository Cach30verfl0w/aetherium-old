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

#include "aetherium/renderer/vulkan_device.hpp"

namespace aetherium::renderer {
    /**
     * This constructor creates an empty vulkan device
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    VulkanDevice::VulkanDevice() noexcept :// NOLINT
            _physical_device {},
            _virtual_device {},
            _properties {} {
    }

    /**
     * This constructor creates the vulkan device by the specified physical device.
     *
     * @param physical_device The handle to the physical device
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    VulkanDevice::VulkanDevice(VkPhysicalDevice physical_device) :// NOLINT
            _physical_device {physical_device} {
        constexpr auto queue_property = 1.0f;
        const std::vector<const char*> device_extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vkGetPhysicalDeviceProperties(_physical_device, &_properties);
        // TODO: Get queue family properties and generate queue store

        VkDeviceQueueCreateInfo device_queue_create_info {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.queueFamilyIndex = 0;
        device_queue_create_info.pQueuePriorities = &queue_property;

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &device_queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.enabledLayerCount = 0;
        device_create_info.enabledExtensionCount = device_extensions.size();
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
        VK_CHECK_EX(vkCreateDevice(_physical_device, &device_create_info, nullptr, &_virtual_device),
                    "Unable to create device: {}")
        volkLoadDevice(_virtual_device);
    }

    VulkanDevice::VulkanDevice(aetherium::renderer::VulkanDevice&& other) noexcept :
            _physical_device {other._physical_device},
            _virtual_device {other._virtual_device},
            _properties {other._properties} {
        other._physical_device = nullptr;
        other._virtual_device = nullptr;
    }

    VulkanDevice::~VulkanDevice() noexcept {
        if(_virtual_device != nullptr) {
            vkDestroyDevice(_virtual_device, nullptr);
            _virtual_device = nullptr;
        }
    }

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
    auto VulkanDevice::emit_command_buffer(F&& function) const noexcept -> kstd::Result<void> {
        static_assert(std::is_convertible_v<F, std::function<void(CommandBuffer&)>>, "Invalid command buffer consumer");

        // Create command buffer and submit fence
        auto command_pool = kstd::try_construct<CommandPool>(this);
        command_pool.throw_if_error();
        auto command_buffer = std::move(command_pool->allocate_command_buffers(1).get_or_throw()[0]);

        VkFence submit_fence {};
        VkFenceCreateInfo submit_fence_create_info {};
        submit_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VK_CHECK(vkCreateFence(_virtual_device, &submit_fence_create_info, nullptr, &submit_fence),
                 "Unable to submit one-time command buffer: {}")

        // Perform operation
        VkCommandBufferBeginInfo command_buffer_begin_info {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(command_buffer._command_buffer, &command_buffer_begin_info),
                 "Unable to submit one-time command buffer: {}")
        function(&command_buffer);
        VK_CHECK(vkEndCommandBuffer(command_buffer._command_buffer), "Unable to submit one-time command buffer: {}")

        // Submit and free
        VkSubmitInfo submit_info {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pCommandBuffers = &command_buffer._command_buffer;
        submit_info.commandBufferCount = 1;

        VkQueue queue {};
        vkGetDeviceQueue(_virtual_device, 0, 0, &queue);
        vkQueueSubmit(queue, 1, &submit_info, submit_fence);
        VK_CHECK(vkWaitForFences(_virtual_device, 1, &submit_fence, true, std::numeric_limits<uint32_t>::max()),
                 "Unable to submit one-time command buffer: {}")
        vkDestroyFence(_virtual_device, submit_fence, nullptr);
        return {};
    }

    auto VulkanDevice::operator=(aetherium::renderer::VulkanDevice&& other) noexcept -> VulkanDevice& {
        _physical_device = other._physical_device;
        _virtual_device = other._virtual_device;
        _properties = other._properties;
        other._physical_device = nullptr;
        other._virtual_device = nullptr;
        return *this;
    }

    /**
     * This constructor creates an empty command buffer
     *
     * @author Cedric Hammes
     * @since  06/02/2024
     */
    CommandBuffer::CommandBuffer() noexcept :// NOLINT
            _command_pool {nullptr},
            _command_buffer {nullptr} {
    }

    /**
     * This constructor creates a command buffer with the specified command buffer and the command pool.
     *
     * @param command_pool   The command pool with which the buffer was allocated
     * @param command_buffer The command buffer itself
     *
     * @author               Cedric Hammes
     * @since                06/02/2024
     */
    CommandBuffer::CommandBuffer(const CommandPool* command_pool, VkCommandBuffer command_buffer) :// NOLINT
            _command_pool {command_pool},
            _command_buffer {command_buffer} {
    }

    CommandBuffer::CommandBuffer(aetherium::renderer::CommandBuffer&& other) noexcept :// NOLINT
            _command_pool {other._command_pool},
            _command_buffer {other._command_buffer} {
        other._command_pool = nullptr;
        other._command_buffer = nullptr;
    }

    CommandBuffer::~CommandBuffer() noexcept {
        if(_command_buffer != nullptr) {
            vkFreeCommandBuffers(_command_pool->_vulkan_device->_virtual_device, _command_pool->_command_pool, 1,
                                         &_command_buffer);
            _command_buffer = nullptr;
        }
    }

    auto CommandBuffer::operator=(aetherium::renderer::CommandBuffer&& other) noexcept -> CommandBuffer& {
        _command_pool = other._command_pool;
        _command_buffer = other._command_buffer;
        other._command_pool = nullptr;
        other._command_buffer = nullptr;
        return *this;
    }

    /**
     * This constructor creates an empty command pool
     *
     * @author Cedric Hammes
     * @since  06/02/2024
     */
    CommandPool::CommandPool() noexcept :// NOLINT
            _vulkan_device {nullptr},
            _command_pool {nullptr} {
    }

    /**
     * This constructor creates a command pool by the specified device
     *
     * @param vulkan_device The device for the pool
     *
     * @author              Cedric Hammes
     * @since               06/02/2024
     */
    CommandPool::CommandPool(const VulkanDevice* vulkan_device) :// NOLINT
            _vulkan_device {vulkan_device},
            _command_pool {} {
        VkCommandPoolCreateInfo command_pool_create_info {};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.queueFamilyIndex = 0;
        VK_CHECK_EX(
                vkCreateCommandPool(vulkan_device->_virtual_device, &command_pool_create_info, nullptr, &_command_pool),
                "Unable to create command pool: {}")
    }

    CommandPool::CommandPool(aetherium::renderer::CommandPool&& other) noexcept ://NOLINT
            _vulkan_device {other._vulkan_device},
            _command_pool {other._command_pool} {
        other._vulkan_device = nullptr;
        other._command_pool = nullptr;
    }

    CommandPool::~CommandPool() noexcept {
        if(_command_pool != nullptr) {
            // TODO: Why is the pool on 0xfd5b260000000001, but Vulkan says 0xfd5b260000000002 is not destroyed?
            vkDestroyCommandPool(_vulkan_device->_virtual_device, _command_pool, nullptr);
            _command_pool = nullptr;
        }
    }

    /**
     * This function allocates the specified count of wrapped command buffers.
     *
     * @param count The count of newly allocated command buffers
     * @return      The command buffers or an error
     *
     * @author      Cedric Hammes
     * @since       06/02/2024
     */
    auto CommandPool::allocate_command_buffers(uint32_t count) const noexcept
            -> kstd::Result<std::vector<CommandBuffer>> {
        VkCommandBufferAllocateInfo allocate_info {};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.commandBufferCount = count;
        allocate_info.commandPool = _command_pool;

        std::vector<VkCommandBuffer> raw_command_buffers {count};
        VK_CHECK(vkAllocateCommandBuffers(_vulkan_device->_virtual_device, &allocate_info, raw_command_buffers.data()),
                 "Unable to allocate buffers {}")

        std::vector<CommandBuffer> command_buffers {};
        command_buffers.reserve(count);
        for(auto raw_command_buffer : raw_command_buffers) {
            command_buffers.emplace_back(this, raw_command_buffer);

        }
        return command_buffers;
    }

    auto CommandPool::operator=(aetherium::renderer::CommandPool&& other) noexcept -> CommandPool& {
        _vulkan_device = other._vulkan_device;
        _command_pool = other._command_pool;
        other._vulkan_device = nullptr;
        other._command_pool = nullptr;
        return *this;
    }
}// namespace aetherium::renderer
