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

#include "aetherium/imgui.hpp"
#include <aetherium/renderer/renderer.hpp>
#include <aetherium/window.hpp>
#include <spdlog/spdlog.h>

using namespace aetherium;

class DefaultScreen final : public Screen {
    renderer::VulkanRenderer* _vulkan_renderer;
    imgui::ImGuiRenderContext* _imgui_context;

    public:
    explicit DefaultScreen(renderer::VulkanRenderer* vulkan_renderer, imgui::ImGuiRenderContext* imgui_context) noexcept
            :
            Screen("Main Menu"),
            _vulkan_renderer {vulkan_renderer},
            _imgui_context {imgui_context} {
    }

    auto render() noexcept -> kstd::Result<void> override {

        if(auto result = _vulkan_renderer->render([&]() {
               _imgui_context->frame([]() {
                   ImGui::ShowUserGuide();
               });
               ImGui::Render();
               ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *_vulkan_renderer->get_command_buffer());
           });
           result.is_error()) {
            return result;
        }
        return {};
    }
};

#undef main
auto main() -> int {
    spdlog::set_level(spdlog::level::debug);
    auto window = Window {"Test window"};
    auto vulkan_context = renderer::VulkanContext {window, "Test App", 1, 0, 0};
    auto renderer = renderer::VulkanRenderer {vulkan_context};
    auto imgui_context = imgui::ImGuiRenderContext {renderer};
    printf("Vulkan Renderer is using the following device: %s\n", renderer.get_device().get_name().c_str());

    window.add_event_handler<ScreenEventHandler>();
    window.add_event_handler<imgui::ImGuiEventHandler>();
    window.set_screen<DefaultScreen>(&renderer, &imgui_context);

    window.run_loop().throw_if_error();
    return 0;
}