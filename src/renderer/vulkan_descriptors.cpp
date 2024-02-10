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

#include "aetherium/renderer/vulkan_descriptors.hpp"

namespace aetherium::renderer {
    auto DescriptorSetBuilder::add_pool_size(VkDescriptorType type, uint32_t count) noexcept -> void {
        _descriptor_pool_size.emplace_back(type, count);
        _general_descriptor_count += count;
    }

    auto DescriptorSetBuilder::build(const VulkanDevice& device) const noexcept -> kstd::Result<VkDescriptorPool> {
        VkDescriptorPoolCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        create_info.poolSizeCount = _descriptor_pool_size.size();
        create_info.pPoolSizes = _descriptor_pool_size.data();
        create_info.maxSets = _general_descriptor_count;

        VkDescriptorPool descriptor_pool {};
        VK_CHECK(vkCreateDescriptorPool(device.get_virtual_device(), &create_info, nullptr, &descriptor_pool),
                 "Unable to create descriptor pool: {}")
        return descriptor_pool;
    }
}// namespace aetherium::renderer