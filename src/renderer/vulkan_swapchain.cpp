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

#include "aetherium/renderer/vulkan_swapchain.hpp"

namespace aetherium::renderer {
    Swapchain::Swapchain() noexcept :// NOLINT
            _vulkan_device {nullptr},
            _swapchain {nullptr},
            _image_views {},
            _images {} {
    }

    Swapchain::Swapchain(const VulkanContext& context, const VulkanDevice* vulkan_device) :// NOLINT
            _vulkan_device {vulkan_device} {
        int32_t width = 0;
        int32_t height = 1;
        SDL_GetWindowSize(context._window->get_window_handle(), &width, &height);
        VkExtent2D window_size {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        auto swapchain_create_info = VkSwapchainCreateInfoKHR {};
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
        VK_CHECK_EX(vkCreateSwapchainKHR(_vulkan_device->_virtual_device, &swapchain_create_info, nullptr, &_swapchain),
                    "Unable to create swapchain: {}")
    }

    Swapchain::~Swapchain() noexcept {
        if(!_image_views.empty()) {
            for(auto& image_view : _image_views) {
                vkDestroyImageView(_vulkan_device->_virtual_device, image_view, nullptr);
            }
            _image_views = {};
        }

        if(!_images.empty()) {
            for(auto& image : _images) {
                vkDestroyImage(_vulkan_device->_virtual_device, image, nullptr);
            }
            _images = {};
        }

        if(_swapchain != nullptr) {
            vkDestroySwapchainKHR(_vulkan_device->_virtual_device, _swapchain, nullptr);
            _swapchain = nullptr;
        }
    }

    Swapchain::Swapchain(aetherium::renderer::Swapchain&& other) noexcept :// NOLINT
            _vulkan_device {other._vulkan_device},
            _swapchain {other._swapchain},
            _image_views {std::move(other._image_views)},
            _images {std::move(other._images)} {
        other._vulkan_device = nullptr;
        other._swapchain = nullptr;
        other._image_views = {};
        other._images = {};
    }

    auto Swapchain::next_image() noexcept -> kstd::Result<void> {
        VK_CHECK(vkAcquireNextImageKHR(_vulkan_device->_virtual_device, _swapchain,
                                       std::numeric_limits<uint64_t>::max(), _vulkan_device->_submit_semaphore,
                                       VK_NULL_HANDLE, &_current_image_index),
                 "Unable to acquire next image: {}")
        return {};
    }

    auto Swapchain::current_image() const noexcept -> VkImage {
        return _images.at(_current_image_index);
    }

    auto Swapchain::current_image_view() const noexcept -> VkImageView {
        return _image_views.at(_current_image_index);
    }

    auto Swapchain::operator=(aetherium::renderer::Swapchain&& other) noexcept -> Swapchain& {
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
}// namespace aetherium::renderer
