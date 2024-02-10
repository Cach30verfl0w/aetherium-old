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

#include "aetherium/renderer/vulkan/swapchain.hpp"

namespace aetherium::renderer::vulkan {
    Swapchain::Swapchain() noexcept :// NOLINT
            _vulkan_device {nullptr},
            _swapchain {nullptr},
            _image_views {},
            _images {} {
    }

    Swapchain::Swapchain(const VulkanContext& context, const VulkanDevice* vulkan_device) :// NOLINT
            _vulkan_device {vulkan_device} {
        // Get window bounds
        int32_t width = 0;
        int32_t height = 1;
        SDL_GetWindowSize(context._window->get_window_handle(), &width, &height);
        VkExtent2D window_size {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        // Create swapchain
        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = context._surface;
        swapchain_create_info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        swapchain_create_info.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchain_create_info.minImageCount = 2;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageExtent = window_size;
        VK_CHECK_EX(vkCreateSwapchainKHR(_vulkan_device->get_virtual_device(), &swapchain_create_info, nullptr, &_swapchain),
                    "Unable to create swapchain: {}")

        // Get images
        uint32_t image_count = 0;
        VK_CHECK_EX(vkGetSwapchainImagesKHR(_vulkan_device->get_virtual_device(), _swapchain, &image_count, nullptr),
                    "Unable to get images: {}")
        _images.resize(image_count);
        _image_views.resize(image_count);
        VK_CHECK_EX(vkGetSwapchainImagesKHR(_vulkan_device->get_virtual_device(), _swapchain, &image_count, _images.data()),
                    "Unable to get images: {}")

        // Create image views from images
        for(auto i = 0; i < _images.size(); i++) {
            VkImageViewCreateInfo image_view_create_info {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image = _images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = VK_FORMAT_B8G8R8A8_UNORM;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;
            VK_CHECK_EX(vkCreateImageView(_vulkan_device->get_virtual_device(), &image_view_create_info, nullptr,
                                          _image_views.data() + i),
                        "Unable to create swapchain: {}")
        }
    }

    Swapchain::~Swapchain() noexcept {
        if(!_image_views.empty()) {
            for(auto& image_view : _image_views) {
                vkDestroyImageView(_vulkan_device->get_virtual_device(), image_view, nullptr);
            }
            _image_views = {};
        }

        if(_swapchain != nullptr) {
            vkDestroySwapchainKHR(_vulkan_device->get_virtual_device(), _swapchain, nullptr);
            _swapchain = nullptr;
        }
    }

    Swapchain::Swapchain(Swapchain&& other) noexcept :// NOLINT
            _vulkan_device {other._vulkan_device},
            _swapchain {other._swapchain},
            _image_views {std::move(other._image_views)},
            _images {std::move(other._images)} {
        other._vulkan_device = nullptr;
        other._swapchain = nullptr;
        other._image_views = {};
        other._images = {};
    }

    auto Swapchain::next_image(VkSemaphore image_available_semaphore) noexcept -> kstd::Result<void> {
        VK_CHECK(vkAcquireNextImageKHR(_vulkan_device->get_virtual_device(), _swapchain,
                                       std::numeric_limits<uint64_t>::max(), image_available_semaphore, VK_NULL_HANDLE,
                                       &_current_image_index),
                 "Unable to acquire next image: {}")
        return {};
    }

    auto Swapchain::current_image() const noexcept -> VkImage {
        return _images.at(_current_image_index);
    }

    auto Swapchain::current_image_view() const noexcept -> VkImageView {
        return _image_views.at(_current_image_index);
    }

    auto Swapchain::current_image_index() const noexcept -> uint32_t {
        return _current_image_index;
    }

    auto Swapchain::operator=(Swapchain&& other) noexcept -> Swapchain& {
        _vulkan_device = other._vulkan_device;
        _swapchain = other._swapchain;
        _images = std::move(other._images);
        _image_views = std::move(other._image_views);
        other._vulkan_device = nullptr;
        other._swapchain = nullptr;
        other._images = {};
        other._image_views = {};
        return *this;
    }

    auto Swapchain::operator*() const noexcept -> VkSwapchainKHR {
        return _swapchain;
    }
}// namespace aetherium::renderer
