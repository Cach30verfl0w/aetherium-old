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
     * This constructor creates the vulkan device by the specified physical device.
     *
     * @param physical_device The handle to the physical device
     *
     * @author Cedric Hammes
     * @since  04/02/2024
     */
    VulkanDevice::VulkanDevice(VkPhysicalDevice physical_device) :// NOLINT
            _physical_device {physical_device},
            _virtual_device {} {
        vkGetPhysicalDeviceProperties(_physical_device, &_properties);
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

    auto VulkanDevice::operator=(aetherium::renderer::VulkanDevice&& other) noexcept -> VulkanDevice& {
        _physical_device = other._physical_device;
        _virtual_device = other._virtual_device;
        _properties = other._properties;
        other._physical_device = nullptr;
        other._virtual_device = nullptr;
        return *this;
    }
}// namespace aetherium::renderer