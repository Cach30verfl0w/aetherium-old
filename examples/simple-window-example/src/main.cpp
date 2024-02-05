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

#include <aetherium/core/window.hpp>
#include <aetherium/core/resource.hpp>
#include <aetherium/renderer/vulkan_context.hpp>

using namespace aetherium;


auto main() -> int {
    auto window = core::Window {"Test window"};
    auto resource_manager = core::ResourceManager {EXAMPLE_DIRECTORY};
    auto vulkan_context = renderer::VulkanContext {window, "Test App", 1, 0, 0};
    auto device = vulkan_context.find_device(renderer::DeviceSearchStrategy::HIGHEST_PERFORMANCE);
    device.throw_if_error();
    printf("Name of the device: %s\n", device->get_name().c_str());
    window.run_loop();
    return 0;
}