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
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <volk.h>

#define UNUSED_PARAMETER(x) (void) (x)

#define VK_CHECK_EX(x, m)                                                                                              \
    if(const auto result = (x); result != VK_SUCCESS) {                                                                \
        throw std::runtime_error {fmt::format((m), get_vulkan_error_message(result))};                                 \
    }

#define VK_CHECK(x, m)                                                                                                 \
    if(const auto result = (x); result != VK_SUCCESS) {                                                                \
        return kstd::Error {fmt::format((m), get_vulkan_error_message(result))};                                       \
    }

namespace aetherium::renderer {
    [[nodiscard]] constexpr auto get_vulkan_error_message(const VkResult result) noexcept -> std::string_view {
        switch(result) {
            case VK_SUCCESS: return "Success";
            case VK_ERROR_LAYER_NOT_PRESENT: return "Layer not present";
            case VK_ERROR_EXTENSION_NOT_PRESENT: return "Extension not present";
            case VK_ERROR_INITIALIZATION_FAILED: return "Initialization failed";
            case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of host memory";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of device memory";
            default: return "Unknown";
        }
    }
}// namespace aetherium::renderer