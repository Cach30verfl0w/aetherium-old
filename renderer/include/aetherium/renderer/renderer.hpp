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
#include <kstd/tuple.hpp>

namespace aetherium::renderer {
    class VulkanRenderer {
        VulkanDevice _vulkan_device;
        CommandPool _command_pool;
        CommandBuffer _command_buffer;
        Swapchain _swapchain;

        public:
        explicit VulkanRenderer(const VulkanContext& context);

        [[nodiscard]] auto get_device() const noexcept -> const VulkanDevice&;
    };

    [[nodiscard]] auto access_mask_flags(VkImageLayout old_layout, VkImageLayout new_layout) noexcept
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