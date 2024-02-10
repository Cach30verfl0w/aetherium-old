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

#include "aetherium/renderer/vulkan_context.hpp"
#include "aetherium/renderer/vulkan_device.hpp"
#include "aetherium/renderer/vulkan_swapchain.hpp"
#include "aetherium/renderer/vulkan_fence.hpp"
#include <kstd/result.hpp>
#include <kstd/tuple.hpp>

namespace aetherium::renderer {
    class VulkanRenderer {
        VulkanContext& _vulkan_context;
        VulkanDevice _vulkan_device;
        CommandPool _command_pool;
        CommandBuffer _command_buffer;
        Swapchain _swapchain;
        VkSemaphore _image_available_semaphore {};
        VkSemaphore _rendering_done_semaphore {};

        public:
        explicit VulkanRenderer(VulkanContext& context);
        VulkanRenderer(VulkanRenderer&& other) noexcept;
        ~VulkanRenderer() noexcept;
        KSTD_NO_COPY(VulkanRenderer, VulkanRenderer);

        template<typename FUNCTION>
        auto render(FUNCTION&& function) noexcept -> kstd::Result<void> {
            static_assert(std::is_convertible_v<FUNCTION, std::function<void()>>, "Invalid function signature");

            VK_CHECK(vkResetCommandPool(_vulkan_device._virtual_device, _command_pool._command_pool,
                                        VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT),
                     "Unable to render: {}")
            VK_CHECK(vkResetCommandBuffer(_command_buffer._command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT),
                     "Unable to render: {}")

            if(const auto next_image_result = _swapchain.next_image(_image_available_semaphore);
               next_image_result.is_error()) {
                return next_image_result;
            }

            // Begin command buffer
            if(const auto begin_result = _command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
               begin_result.is_error()) {
                return begin_result;
            }

            VkImageMemoryBarrier image_memory_barrier {};
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            image_memory_barrier.image = _swapchain.current_image();
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(_command_buffer._command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1,
                                 &image_memory_barrier);

            // Get window bounds
            int32_t width = 0;
            int32_t height = 1;
            SDL_GetWindowSize(_vulkan_context.get_window()->get_window_handle(), &width, &height);
            VkExtent2D window_size {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            // Get rendering info
            VkRenderingAttachmentInfo attachment_info {};
            attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachment_info.imageView = _swapchain.current_image_view();
            attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_info.clearValue.color.float32[0] = 0.0f;
            attachment_info.clearValue.color.float32[1] = 0.0f;
            attachment_info.clearValue.color.float32[2] = 0.0f;
            attachment_info.clearValue.color.float32[3] = 1.0f;

            VkRenderingInfo rendering_info {};
            rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
            rendering_info.renderArea.extent = window_size;
            rendering_info.colorAttachmentCount = 1;
            rendering_info.pColorAttachments = &attachment_info;
            rendering_info.layerCount = 1;

            vkCmdBeginRendering(_command_buffer._command_buffer, &rendering_info);
            function();
            vkCmdEndRendering(_command_buffer._command_buffer);

            image_memory_barrier = {};
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            image_memory_barrier.image = _swapchain.current_image();
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = 1;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(_command_buffer._command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

            // End command buffer
            if(const auto end_result = _command_buffer.end(); end_result.is_error()) {
                return end_result;
            }

            VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            const auto fence = VulkanFence {&_vulkan_device};

            VkSubmitInfo submit_info {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = &_image_available_semaphore;
            submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &_command_buffer._command_buffer;
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = &_rendering_done_semaphore;
            VK_CHECK(vkQueueSubmit(_vulkan_device._graphics_queue, 1, &submit_info, *fence), "Unable to submit: {}")

            auto current_image_index = _swapchain.current_image_index();
            VkPresentInfoKHR present_info {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = &_rendering_done_semaphore;
            present_info.swapchainCount = 1;
            present_info.pSwapchains = &_swapchain._swapchain;
            present_info.pImageIndices = &current_image_index;
            VK_CHECK(vkQueuePresentKHR(_vulkan_device._graphics_queue, &present_info), "Unable to present queue: {}")
            if(const auto wait_result = fence.wait_for(); wait_result.is_error()) {
                return wait_result;
            }

            return {};
        }

        [[nodiscard]] auto get_device() const noexcept -> const VulkanDevice&;
        [[nodiscard]] auto get_context() const noexcept -> const VulkanContext&;
        [[nodiscard]] auto get_swapchain() const noexcept -> const Swapchain&;
        [[nodiscard]] auto get_command_buffer() const noexcept -> const CommandBuffer&;

        auto operator=(VulkanRenderer&& other) noexcept -> VulkanRenderer&;
    };

    [[nodiscard]] auto access_mask_flags(VkImageLayout old_layout, VkImageLayout new_layout) noexcept
            -> kstd::Option<kstd::Tuple<VkAccessFlags, VkAccessFlags>>;

}// namespace aetherium::renderer