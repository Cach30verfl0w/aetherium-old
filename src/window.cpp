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
    Window::Window(std::string_view window_title, int32_t width, int32_t height) {
        using namespace std::string_literals;

        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error {fmt::format("Unable to init window: {}", SDL_GetError())};
        }

        _window_handle = SDL_CreateWindow(window_title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
                                          height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
        if(!_window_handle) {
            throw std::runtime_error {fmt::format("Unable to init window: {}", SDL_GetError())};
        }

        SDL_ShowWindow(_window_handle);
    }

    Window::Window(aetherium::Window&& other) noexcept :// NOLINT
            _window_handle {other._window_handle} {
        other._window_handle = nullptr;
    }

    Window::~Window() noexcept {
        if(_window_handle != nullptr) {
            SDL_DestroyWindow(_window_handle);
            SDL_Quit();
            _window_handle = nullptr;
        }
    }

    auto Window::run_loop() const noexcept -> kstd::Result<void> {
        SDL_Event event {};
        while (true) {
            while (SDL_PollEvent(&event)) {
                // Close screen if event is quit event
                if (event.type == SDL_QUIT) {
                    return {};
                }

                // Notify handler about event
                for(const auto& event_handler : this->_event_handlers) {
                    if (const auto result = event_handler->handle_event(&event); result.is_error()) {
                        return result;
                    }
                }

                // Redraw screen
                if (_current_screen.has_value()) {
                    if (const auto result = (*_current_screen)->render(); result.is_error()) {
                        return result;
                    }
                }
            }
        }
    }

    auto Window::get_window_handle() noexcept -> SDL_Window* {
        return _window_handle;
    }

    auto Window::operator=(aetherium::Window&& other) noexcept -> Window& {
        _window_handle = other._window_handle;
        other._window_handle = nullptr;
        return *this;
    }
}// namespace aetherium::core