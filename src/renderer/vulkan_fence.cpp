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

#include "aetherium/renderer/vulkan_fence.hpp"

namespace aetherium::renderer {

    /**
     * This constructor creates the fence by the specified device. This fence is used to wait on the CPU-site for
     * operations on the GPU.
     *
     * @param device The device on which the fence is to be created
     *
     * @author       Cedric Hammes
     * @since        09/02/2024
     */
    VulkanFence::VulkanFence(const aetherium::renderer::VulkanDevice* device) :
            _device {device},
            _fence_handle {nullptr} {
        VkFenceCreateInfo fence_create_info = {};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VK_CHECK_EX(vkCreateFence(_device->_virtual_device, &fence_create_info, nullptr, &_fence_handle),
                    "Unable to create fence: {}")
    }

    VulkanFence::VulkanFence(aetherium::renderer::VulkanFence&& other) noexcept :
            _device {other._device},
            _fence_handle {other._fence_handle} {
        other._device = nullptr;
        other._fence_handle = nullptr;
    }

    VulkanFence::~VulkanFence() noexcept {
        if(_fence_handle != nullptr) {
            vkDestroyFence(_device->_virtual_device, _fence_handle, nullptr);
            _fence_handle = nullptr;
        }
    }

    /**
     * This function waits for a signal, which indicates that the task is ended, by the fence. We are waiting based
     * on the specified timeout.
     *
     * @param timeout The maximal wait timeout
     * @return        Success or error
     *
     * @author        Cedric Hammes
     * @since         09/02/2024
     */
    auto VulkanFence::wait_for(uint64_t timeout) const noexcept -> kstd::Result<void> {
        VK_CHECK(vkWaitForFences(_device->_virtual_device, 1, &_fence_handle, true, timeout),
                 "Unable to wait for fence: {}")
        return {};
    }

    auto VulkanFence::operator=(aetherium::renderer::VulkanFence&& other) noexcept -> VulkanFence& {
        _device = other._device;
        _fence_handle = other._fence_handle;
        other._device = nullptr;
        other._fence_handle = nullptr;
        return *this;
    }

    auto VulkanFence::operator*() const noexcept -> VkFence {
        return _fence_handle;
    }
}// namespace aetherium::renderer