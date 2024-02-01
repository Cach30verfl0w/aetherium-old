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
#include <SDL2/SDL.h>
#include <fmt/format.h>
#include <kstd/defaults.hpp>
#include <kstd/result.hpp>
#include <stdexcept>

namespace aetherium {
    // This is a
    class Window final {
        SDL_Window* _window_handle {};
        SDL_Surface* _screen_surface {};

        public:
        explicit Window(std::string_view window_title, int32_t width = 800, int32_t height = 600);
        Window(const Window& other) = delete;
        Window(Window&& other) noexcept;
        ~Window() noexcept;

        [[nodiscard]] auto handle_event(const SDL_Event* event) const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto run_loop() const noexcept -> kstd::Result<void>;
    };
}// namespace aetherium