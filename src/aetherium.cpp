// Copyright 2024 Karma Krafts & associates
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

#include "aetherium/aetherium.hpp"
#include <stdexcept>

namespace aetherium {
     Application::Application(const std::string& name, const uint32_t version) {
         VkApplicationInfo application_info {};
         application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
         application_info.apiVersion = VK_API_VERSION_1_3; // Newest vulkan version
         application_info.pEngineName = "Aetherium Engine";
         application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
         application_info.pApplicationName = name.data();
         application_info.applicationVersion = version;
         application_info.pNext = nullptr;

         VkInstanceCreateInfo instance_create_info {};
         instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
         instance_create_info.pApplicationInfo = &application_info;
         instance_create_info.enabledExtensionCount = 0;
         instance_create_info.enabledLayerCount = 0;
         instance_create_info.pNext = nullptr;
         VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &_vulkan_instance), "Unable to init app: {}");
    }

    auto vk_error_message(const VkResult result) noexcept -> std::string {
         switch(result) {
             case VK_SUCCESS: return "Succeded";
             case VK_ERROR_DEVICE_LOST: return "Device lost";
             case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of host memory";
             case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of device memory";
             case VK_ERROR_INCOMPATIBLE_DRIVER: return "Incompatible driver";
             case VK_ERROR_MEMORY_MAP_FAILED: return "Memory map failed";
             case VK_ERROR_INITIALIZATION_FAILED: return "Initialization failed";
             default: return "Unknown/Unidentified";
         }
    }

}