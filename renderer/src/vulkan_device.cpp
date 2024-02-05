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
            _physical_device {physical_device} {
        constexpr auto queue_property = 1.0f;

        vkGetPhysicalDeviceProperties(_physical_device, &_properties);
        // TODO: Get queue family properties and generate queue store

        VkDeviceQueueCreateInfo device_queue_create_info {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.queueFamilyIndex = 0;
        device_queue_create_info.pQueuePriorities = &queue_property;

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &device_queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.enabledLayerCount = 0;
        device_create_info.enabledExtensionCount = 0;
        VK_CHECK_EX(vkCreateDevice(_physical_device, &device_create_info, nullptr, &_virtual_device),
                    "Unable to create device: {}")
        volkLoadDevice(_virtual_device);
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

    /**
     * This function enumerates all available device queues and returns the first available device for the specific
     * task. TODO: Implement queue enumeration function
     *
     * @param queue_flags The flags of the wanted queue
     * @return            The queue or a none
     *
     * @author Cedric Hammes
     * @since  05/02/2024
     */
    auto VulkanDevice::acquire_queue(VkQueueFlagBits queue_flags) const noexcept -> kstd::Option<VkQueue> {
        UNUSED_PARAMETER(queue_flags);

        VkQueue queue {};
        vkGetDeviceQueue(_virtual_device, 0, 0, &queue);
        return queue;
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