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
#include "aetherium/renderer/vulkan/fence.hpp"
#include <array>

namespace aetherium::renderer {
    VulkanRenderer::VulkanRenderer(vulkan::VulkanContext& context) :
            _vulkan_context {context},
            _vulkan_device {} {
        _vulkan_device =
                std::move(context.find_device(vulkan::DeviceSearchStrategy::HIGHEST_PERFORMANCE).get_or_throw());
        _command_pool = vulkan::CommandPool {&_vulkan_device};
        _command_buffer = std::move(_command_pool.allocate_command_buffers(1).get_or_throw().at(0));
        _swapchain = vulkan::Swapchain {context, &_vulkan_device};

        // Create semaphores
        VkSemaphoreCreateInfo semaphore_create_info {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK_EX(vkCreateSemaphore(_vulkan_device.get_virtual_device(), &semaphore_create_info, nullptr,
                                      &_image_available_semaphore),
                    "Unable to create device: {}")
        VK_CHECK_EX(vkCreateSemaphore(_vulkan_device.get_virtual_device(), &semaphore_create_info, nullptr,
                                      &_rendering_done_semaphore),
                    "Unable to create device: {}")
    }

    VulkanRenderer::VulkanRenderer(aetherium::renderer::VulkanRenderer&& other) noexcept :
            _vulkan_context {other._vulkan_context},
            _vulkan_device {std::move(other._vulkan_device)},
            _command_pool {std::move(other._command_pool)},
            _command_buffer {std::move(other._command_buffer)},
            _swapchain {std::move(other._swapchain)},
            _image_available_semaphore {other._image_available_semaphore},
            _rendering_done_semaphore {other._rendering_done_semaphore} {
        other._image_available_semaphore = nullptr;
        other._rendering_done_semaphore = nullptr;
    }

    VulkanRenderer::~VulkanRenderer() noexcept {
        if(_image_available_semaphore != nullptr) {
            vkDestroySemaphore(_vulkan_device.get_virtual_device(), _image_available_semaphore, nullptr);
            _image_available_semaphore = nullptr;
        }

        if(_rendering_done_semaphore != nullptr) {
            vkDestroySemaphore(_vulkan_device.get_virtual_device(), _rendering_done_semaphore, nullptr);
            _rendering_done_semaphore = nullptr;
        }
    }

    auto VulkanRenderer::render() noexcept -> kstd::Result<void> {
        VK_CHECK(vkResetCommandPool(_vulkan_device.get_virtual_device(), *_command_pool,
                                    VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT),
                 "Unable to render: {}")
        VK_CHECK(vkResetCommandBuffer(*_command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT),
                 "Unable to render: {}")
        auto command_buffer = *_command_buffer;

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
        vkCmdPipelineBarrier(*_command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
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

        vkCmdBeginRendering(*_command_buffer, &rendering_info);
        // TODO: Render
        vkCmdEndRendering(*_command_buffer);

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
        vkCmdPipelineBarrier(*_command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

        // End command buffer
        if(const auto end_result = _command_buffer.end(); end_result.is_error()) {
            return end_result;
        }

        VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        const auto fence = vulkan::VulkanFence {&_vulkan_device};

        VkSubmitInfo submit_info {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &_image_available_semaphore;
        submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &_rendering_done_semaphore;
        VK_CHECK(vkQueueSubmit(_vulkan_device.get_graphics_queue(), 1, &submit_info, *fence), "Unable to submit: {}")
        if(const auto wait_result = fence.wait_for(); wait_result.is_error()) {
            return wait_result;
        }

        auto current_image_index = _swapchain.current_image_index();
        auto raw_swapchain_handle = *_swapchain;
        VkPresentInfoKHR present_info {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &_rendering_done_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &raw_swapchain_handle;
        present_info.pImageIndices = &current_image_index;
        VK_CHECK(vkQueuePresentKHR(_vulkan_device.get_graphics_queue(), &present_info), "Unable to present queue: {}")

        return {};
    }

    auto VulkanRenderer::get_device() const noexcept -> const vulkan::VulkanDevice& {
        return _vulkan_device;
    }

    auto VulkanRenderer::operator=(aetherium::renderer::VulkanRenderer&& other) noexcept -> VulkanRenderer& {
        _vulkan_context = std::move(other._vulkan_context);
        _vulkan_device = std::move(other._vulkan_device);
        _command_pool = std::move(other._command_pool);
        _command_buffer = std::move(other._command_buffer);
        _swapchain = std::move(other._swapchain);
        _image_available_semaphore = other._image_available_semaphore;
        _rendering_done_semaphore = other._rendering_done_semaphore;
        return *this;
    }

    auto access_mask_flags(VkImageLayout old_layout, VkImageLayout new_layout) noexcept
            -> kstd::Option<kstd::Tuple<VkAccessFlags, VkAccessFlags>> {
        if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            return {{VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT}};
        }
        else if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            return {{VK_ACCESS_NONE, VK_ACCESS_NONE}};
        }
        else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            return {{VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT}};
        }
        else if(old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
                new_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
            return {{VK_ACCESS_NONE, VK_ACCESS_NONE}};
        }
        return kstd::Option<kstd::Tuple<VkAccessFlags, VkAccessFlags>> {};
    }
}// namespace aetherium::renderer