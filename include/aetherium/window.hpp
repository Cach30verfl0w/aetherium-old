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
#include "aetherium/screens.hpp"
#include <SDL2/SDL.h>
#include <fmt/format.h>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <kstd/result.hpp>
#include <kstd/tuple.hpp>
#include <stdexcept>
#include <string>

namespace aetherium {
    class Window;

    class EventHandler {
        public:
        virtual ~EventHandler() noexcept = default;
        [[nodiscard]] virtual auto handle_event(const Window* window, SDL_Event* event) -> kstd::Result<void> = 0;
    };

    class ScreenEventHandler final : public EventHandler {
        public:
        ScreenEventHandler() noexcept = default;
        ~ScreenEventHandler() noexcept override = default;
        KSTD_DEFAULT_MOVE_COPY(ScreenEventHandler, ScreenEventHandler);
        auto handle_event(const aetherium::Window* window, SDL_Event* event) -> kstd::Result<void> override;
    };

    class Window final {
        std::string_view _window_name;
        SDL_Window* _window_handle;
        std::vector<std::unique_ptr<EventHandler>> _event_handlers {};
        kstd::Option<std::shared_ptr<Screen>> _current_screen {};

        public:
        explicit Window(std::string_view window_title, int32_t width = 800, int32_t height = 600);
        Window(Window&& other) noexcept;
        ~Window() noexcept;
        KSTD_NO_COPY(Window, Window)

        template<typename SCREEN, typename... ARGS>
        [[nodiscard]] auto set_screen(ARGS&&... args) {
            static_assert(std::is_base_of_v<Screen, SCREEN>, "Specified screen isn't a real screen!");
            const auto screen = std::make_shared<SCREEN>(std::forward<ARGS>(args)...);
            const std::string window_title = {fmt::format("{} - {}", _window_name, (*screen).get_name())};
            SDL_SetWindowTitle(_window_handle, window_title.data());
            _current_screen = {screen};
        }

        template<typename HANDLER, typename... ARGS>
        [[nodiscard]] auto add_event_handler(ARGS&&... args) noexcept {
            static_assert(std::is_base_of_v<EventHandler, HANDLER>, "Specified handler isn't a real event handler!");
            _event_handlers.push_back(std::make_unique<HANDLER>(std::forward<ARGS>(args)...));
        }

        [[nodiscard]] auto run_loop() const noexcept -> kstd::Result<void>;
        [[nodiscard]] auto get_window_handle() noexcept -> SDL_Window*;
        [[nodiscard]] auto get_current_screen() const noexcept -> kstd::Option<std::shared_ptr<Screen>>;
        auto operator=(Window&& other) noexcept -> Window&;
    };
}// namespace aetherium
