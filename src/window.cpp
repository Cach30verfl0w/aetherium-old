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

#include "aetherium/window.hpp"

namespace aetherium {
    Window::Window(std::string_view window_title, int32_t width, const int32_t height) {
        // Initialize window handle
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error {fmt::format("Unable to initialize SDL window: {}", SDL_GetError())};
        }
        _window_handle = SDL_CreateWindow(window_title.data(), SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

        _screen_surface = SDL_GetWindowSurface(_window_handle);
        SDL_FillRect(_screen_surface, nullptr, SDL_MapRGB(_screen_surface->format, 0x00, 0x00, 0x00));
        SDL_UpdateWindowSurface(_window_handle);
    }

    Window::Window(aetherium::Window&& other) noexcept ://NOLINT
            _window_handle {other._window_handle} {
        other._window_handle = nullptr;
    }

    Window::~Window() noexcept {
        if (_window_handle == nullptr) {
            return;
        }

        SDL_DestroyWindow(_window_handle);
        SDL_Quit();
    }

    auto Window::handle_event(const SDL_Event* event) const noexcept -> kstd::Result<void> {
        switch (event->type) {
            default: return {};
        }
        return {};
    }

    auto Window::run_loop() const noexcept -> kstd::Result<void> {
        SDL_Event event {};
        while (true) {
            // Wait for event
            while(SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    return {};
                }

                const auto result = handle_event(&event);
                if(result.is_error()) {
                    return result;
                }
            }
        }
    }
}