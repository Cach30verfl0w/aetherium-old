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

#pragma once
#include "aetherium/renderer/vulkan_device.hpp"

namespace aetherium::renderer {
    class DescriptorSetBuilder {
        uint32_t _general_descriptor_count = 0;
        std::vector<VkDescriptorPoolSize> _descriptor_pool_size {};

        public:
        DescriptorSetBuilder() noexcept = default;
        ~DescriptorSetBuilder() noexcept = default;
        KSTD_DEFAULT_MOVE_COPY(DescriptorSetBuilder, DescriptorSetBuilder);

        auto add_pool_size(VkDescriptorType type, uint32_t count) noexcept -> void;
        [[nodiscard]] auto build(const VulkanDevice& device) const noexcept -> kstd::Result<VkDescriptorPool>;
    };
}