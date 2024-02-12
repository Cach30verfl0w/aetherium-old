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
#include "aetherium/renderer/vulkan/context.hpp"
#include "aetherium/renderer/vulkan/device.hpp"

namespace aetherium::renderer::vulkan {
    class Swapchain final {
        const VulkanDevice* _vulkan_device;
        VkSwapchainKHR _swapchain;
        std::vector<VkImageView> _image_views {};
        std::vector<VkImage> _images {};
        uint32_t _current_image_index;
        uint32_t _image_count = 2;

        public:
        friend class VulkanRenderer;

        Swapchain() noexcept;
        explicit Swapchain(const VulkanContext& context, const VulkanDevice* vulkan_device);
        Swapchain(Swapchain&& other) noexcept;
        ~Swapchain() noexcept;
        KSTD_NO_COPY(Swapchain, Swapchain);

        [[nodiscard]] auto next_image(VkSemaphore image_available_semaphore) noexcept -> kstd::Result<void>;
        [[nodiscard]] auto current_image() const noexcept -> VkImage;
        [[nodiscard]] auto current_image_view() const noexcept -> VkImageView;
        [[nodiscard]] auto current_image_index() const noexcept -> uint32_t;
        [[nodiscard]] auto get_image_count() const noexcept -> uint32_t;

        auto operator=(Swapchain&& other) noexcept -> Swapchain&;
        auto operator*() const noexcept -> VkSwapchainKHR;
    };
}// namespace aetherium::renderer::vulkan
