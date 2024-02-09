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
#include <kstd/result.hpp>

namespace aetherium::renderer {
    class VulkanRenderer {
        VulkanContext& _vulkan_context;
        VulkanDevice _vulkan_device;
        CommandPool _command_pool;
        CommandBuffer _command_buffer;
        Swapchain _swapchain;

        public:
        explicit VulkanRenderer(VulkanContext& context);

        [[nodiscard]] auto render() noexcept -> kstd::Result<void>;

        [[nodiscard]] auto get_device() const noexcept -> const VulkanDevice&;
    };

    [[nodiscard]] auto access_mask_flags(VkImageLayout old_layout, VkImageLayout new_layout) noexcept
            -> kstd::Option<kstd::Tuple<VkAccessFlags, VkAccessFlags>>;

}// namespace aetherium::renderer