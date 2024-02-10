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

#include "aetherium/imgui.hpp"

namespace aetherium::imgui {
    ImGuiRenderContext::ImGuiRenderContext(renderer::VulkanRenderer& renderer) :
            _window {renderer.get_context().get_window()},
            _imgui_shown {false} {
        IMGUI_CHECKVERSION();
        _imgui_context = ImGui::CreateContext();
        if(!_imgui_context) {
            throw std::runtime_error("Initialization of ImGui Context failed: Unable to acquire context");
        }

        if(!ImGui_ImplSDL2_InitForVulkan(renderer.get_context().get_window()->get_window_handle())) {
            throw std::runtime_error(
                    "Initialization of ImGui Context failed: Unable to init ImGui for Vulkan with SDL2");
        }

        // Init vulkan
        const auto surface_caps =
                renderer.get_context().get_surface_properties(renderer.get_device()).get_or_throw().surfaceCapabilities;

        ImGui_ImplVulkan_InitInfo vulkan_init_info {};
        vulkan_init_info.Instance = *renderer.get_context();
        vulkan_init_info.PhysicalDevice = renderer.get_device().get_physical_device();
        vulkan_init_info.Device = renderer.get_device().get_virtual_device();
        vulkan_init_info.QueueFamily = 0;
        vulkan_init_info.Queue = renderer.get_device().get_graphics_queue();
        vulkan_init_info.PipelineCache = VK_NULL_HANDLE;
        vulkan_init_info.DescriptorPool = renderer.get_device().get_descriptor_pool();
        vulkan_init_info.Subpass = 0;
        vulkan_init_info.MinImageCount = surface_caps.minImageCount < 2 ? 2 : surface_caps.minImageCount;
        vulkan_init_info.ImageCount = renderer.get_swapchain().get_image_count();
        vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        vulkan_init_info.Allocator = nullptr;
        vulkan_init_info.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
        vulkan_init_info.UseDynamicRendering = true;

        ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* instance) {
            return vkGetInstanceProcAddr(*static_cast<renderer::VulkanRenderer*>(instance)->get_context(), function_name);
        }, &renderer);
        if(!ImGui_ImplVulkan_Init(&vulkan_init_info, nullptr)) {
            throw std::runtime_error("Initialization of ImGui Context failed: Unable to init ImGui for Vulkan");
        }
    }

    ImGuiRenderContext::ImGuiRenderContext(aetherium::imgui::ImGuiRenderContext&& other) noexcept :
            _imgui_context {other._imgui_context},
            _window {other._window},
            _imgui_shown {other._imgui_shown} {
        other._imgui_context = nullptr;
        other._window = nullptr;
        other._imgui_shown = false;
    }

    ImGuiRenderContext::~ImGuiRenderContext() noexcept {
        if(_imgui_context != nullptr) {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext(_imgui_context);
        }
    }

    auto ImGuiEventHandler::handle_event(const aetherium::Window* window, SDL_Event* event) -> kstd::Result<void> {
        ImGui_ImplSDL2_ProcessEvent(event);
    }

    auto ImGuiRenderContext::operator=(aetherium::imgui::ImGuiRenderContext&& other) noexcept -> ImGuiRenderContext& {
        _imgui_context = other._imgui_context;
        _window = other._window;
        _imgui_shown = other._imgui_shown;
        other._imgui_context = nullptr;
        other._window = nullptr;
        return *this;
    }
}// namespace aetherium::imgui