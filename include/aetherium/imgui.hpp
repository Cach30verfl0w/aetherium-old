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
#include "aetherium/renderer/renderer.hpp"
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>

namespace aetherium::imgui {
    class ImGuiRenderContext final {
        ImGuiContext* _imgui_context;
        Window* _window;
        bool _imgui_shown;

        public:
        explicit ImGuiRenderContext(renderer::VulkanRenderer& renderer);
        ImGuiRenderContext(ImGuiRenderContext&& other) noexcept;
        ~ImGuiRenderContext() noexcept;
        KSTD_NO_COPY(ImGuiRenderContext, ImGuiRenderContext);

        template<typename FUNCTION>
        auto frame(FUNCTION&& function) const noexcept -> void {
            static_assert(std::is_convertible_v<FUNCTION, std::function<void()>>, "Invalid function signature");
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL2_NewFrame(_window->get_window_handle());
            ImGui::NewFrame();
            function();
            ImGui::EndFrame();

            auto& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }

        auto operator=(ImGuiRenderContext&& other) noexcept -> ImGuiRenderContext&;
    };

    class ImGuiEventHandler final : public EventHandler {
        public:
        ImGuiEventHandler() noexcept = default;
        ~ImGuiEventHandler() noexcept override = default;
        auto handle_event(const aetherium::Window *window, SDL_Event *event) -> kstd::Result<void> override;
    };
}