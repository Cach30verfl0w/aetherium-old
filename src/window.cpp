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
    Window::Window(std::string_view window_title, int32_t width, int32_t height) :
            _window_name {window_title} {
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
            _window_name {other._window_name},
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
        auto is_running = true;
        while(is_running) {
            while(is_running && SDL_PollEvent(&event)) {
                // Close screen if event is quit event
                if(event.type == SDL_QUIT) {
                    is_running = false;
                    continue;
                }

                // Notify handler about event
                for(const auto& event_handler : this->_event_handlers) {
                    if(const auto result = event_handler->handle_event(this, &event); result.is_error()) {
                        return result;
                    }
                }

                // Redraw screen
                if(_current_screen.has_value()) {
                    if(const auto result = (*_current_screen)->render(); result.is_error()) {
                        return result;
                    }
                }
            }
        }
        return {};
    }

    auto Window::get_window_handle() noexcept -> SDL_Window* {
        return _window_handle;
    }

    auto Window::get_current_screen() const noexcept -> kstd::Option<std::shared_ptr<Screen>> {
        if(_current_screen.is_empty()) {
            return kstd::Option<std::shared_ptr<Screen>> {};
        }
        return {*_current_screen};
    }

    auto Window::operator=(aetherium::Window&& other) noexcept -> Window& {
        _window_name = other._window_name;
        _window_handle = other._window_handle;
        other._window_handle = nullptr;
        return *this;
    }

    auto ScreenEventHandler::handle_event(const aetherium::Window* window, SDL_Event* event) -> kstd::Result<void> {
        auto screen = window->get_current_screen();
        if(screen.has_value()) {
            if(const auto render_result = screen.get()->render(); render_result.is_error()) {
                return render_result;
            }
        }

        switch(event->type) {
            default: return {};
        }
    }
}// namespace aetherium