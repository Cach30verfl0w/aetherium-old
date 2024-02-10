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
#include <array>

namespace aetherium::renderer {
    VulkanRenderer::VulkanRenderer(aetherium::renderer::VulkanContext& context) :
            _vulkan_context {context},
            _vulkan_device {} {
        _vulkan_device = std::move(context.find_device(DeviceSearchStrategy::HIGHEST_PERFORMANCE).get_or_throw());
        _command_pool = CommandPool {&_vulkan_device};
        _command_buffer = std::move(_command_pool.allocate_command_buffers(1).get_or_throw().at(0));
        _swapchain = Swapchain {context, &_vulkan_device};

        // Create semaphores
        VkSemaphoreCreateInfo semaphore_create_info {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK_EX(vkCreateSemaphore(_vulkan_device._virtual_device, &semaphore_create_info, nullptr,
                                      &_image_available_semaphore),
                    "Unable to create device: {}")
        VK_CHECK_EX(vkCreateSemaphore(_vulkan_device._virtual_device, &semaphore_create_info, nullptr,
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
            vkDestroySemaphore(_vulkan_device._virtual_device, _image_available_semaphore, nullptr);
            _image_available_semaphore = nullptr;
        }

        if(_rendering_done_semaphore != nullptr) {
            vkDestroySemaphore(_vulkan_device._virtual_device, _rendering_done_semaphore, nullptr);
            _rendering_done_semaphore = nullptr;
        }
    }

    auto VulkanRenderer::get_device() const noexcept -> const VulkanDevice& {
        return _vulkan_device;
    }

    auto VulkanRenderer::get_context() const noexcept -> const VulkanContext& {
        return _vulkan_context;
    }

    auto VulkanRenderer::get_swapchain() const noexcept -> const Swapchain& {
        return _swapchain;
    }

    auto VulkanRenderer::get_command_buffer() const noexcept -> const CommandBuffer& {
        return _command_buffer;
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