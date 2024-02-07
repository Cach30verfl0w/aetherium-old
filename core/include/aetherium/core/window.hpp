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
#include <SDL.h>
#include <fmt/format.h>
#include <kstd/defaults.hpp>
#include <kstd/result.hpp>
#include <kstd/tuple.hpp>
#include <stdexcept>
#include <string>

namespace aetherium::core {
    struct WindowSize final {
        int32_t width;
        int32_t height;
    };

    class Window final {
        SDL_Window* _window_handle;

        public:
        explicit Window(std::string_view window_title, int32_t width = 800, int32_t height = 600);
        Window(Window&& other) noexcept;
        ~Window() noexcept;
        KSTD_NO_COPY(Window, Window)

        [[nodiscard]] auto handle_event(const SDL_Event* event) const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto run_loop() const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto get_window_handle() noexcept -> SDL_Window*;
        auto operator=(Window&& other) noexcept -> Window&;
    };
}// namespace aetherium::core
