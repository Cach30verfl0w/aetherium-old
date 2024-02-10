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
#include "aetherium/renderer/vulkan/device.hpp"

namespace aetherium::renderer::vulkan {
    /**
     * This class is a safe-wrapper around the vulkan fence. This allows the developer to wait on the CPU-site for
     * GPU-site operations.
     *
     * @author Cedric Hammes
     * @since  09/02/2024
     */
    class VulkanFence {
        const VulkanDevice* _device;
        VkFence _fence_handle;

        public:
        /**
         * This constructor creates the fence by the specified device. This fence is used to wait on the CPU-site for
         * operations on the GPU.
         *
         * @param device The device on which the fence is to be created
         *
         * @author       Cedric Hammes
         * @since        09/02/2024
         */
        explicit VulkanFence(const VulkanDevice* device);
        ~VulkanFence() noexcept;
        VulkanFence(VulkanFence&& other) noexcept;
        KSTD_NO_COPY(VulkanFence, VulkanFence);

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
        [[nodiscard]] auto wait_for(uint64_t timeout = std::numeric_limits<uint64_t>::max()) const noexcept
                -> kstd::Result<void>;

        auto operator=(VulkanFence&& other) noexcept -> VulkanFence&;
        auto operator*() const noexcept -> VkFence;
    };
}// namespace aetherium::renderer