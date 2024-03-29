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

#include <aetherium/renderer/renderer.hpp>
#include <aetherium/window.hpp>
#include <spdlog/spdlog.h>

using namespace aetherium;

class DefaultScreen final : public Screen {
    renderer::VulkanRenderer* _vulkan_renderer;

    public:
    explicit DefaultScreen(renderer::VulkanRenderer* vulkan_renderer) noexcept
            :
            Screen("Main Menu"),
            _vulkan_renderer {vulkan_renderer} {
    }

    auto render() noexcept -> kstd::Result<void> override {
        if(auto result = _vulkan_renderer->render(); result.is_error()) {
            return result;
        }
        return {};
    }
};

#undef main
auto main() -> int {
    spdlog::set_level(spdlog::level::debug);
    auto window = Window {"Test window"};
    auto vulkan_context = renderer::vulkan::VulkanContext {window, "Test App", 1, 0, 0};
    auto renderer = renderer::VulkanRenderer {vulkan_context};
    printf("Vulkan Renderer is using the following device: %s\n", renderer.get_device().get_name().c_str());

    window.add_event_handler<ScreenEventHandler>();
    window.set_screen<DefaultScreen>(&renderer);

    window.run_loop().throw_if_error();
    return 0;
}