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
#include <aetherium/renderer/shader.hpp>
#include <aetherium/resource.hpp>
#include <aetherium/window.hpp>

using namespace aetherium;
using namespace aetherium::renderer;

auto main() -> int {
    auto window = Window {"Example Window"};
    auto context = VulkanContext {window, "Test application", VK_MAKE_VERSION(1, 0, 0)};
    auto device = context.find_device(DeviceSearchStrategy::HIGHEST_PERFORMANCE);

    auto resource_manager = ResourceManager {EXAMPLE_APP_PATH};
    auto shader = resource_manager.load_resource<Shader>("test", "shader.glsl", ShaderKind::VERTEX);
    shader.throw_if_error();
    printf("Selected device %s and shader found at %s\n", device->get_name().c_str(),
           shader->get_resource_path().c_str());

    window.run_loop().throw_if_error();
}
