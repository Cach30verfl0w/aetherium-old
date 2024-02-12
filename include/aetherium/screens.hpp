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
#include <kstd/result.hpp>
#include <string>

namespace aetherium {
    class Screen {
        std::string _screen_name;

        public:
        /**
         * This constructor initializes the screen with the screen name
         *
         * @param screen_name The name of the screen
         *
         * @author            Cedric Hammes
         * @since             07/02/2024
         */
        explicit Screen(std::string screen_name) noexcept;
        virtual ~Screen() noexcept = default;
        KSTD_DEFAULT_MOVE_COPY(Screen, Screen);

        /**
         * This function is called before the screen is set to this.
         *
         * @return Void or an error
         *
         * @author Cedric Hammes
         * @since  07/02/2024
         */
        [[nodiscard]] virtual auto init() noexcept -> kstd::Result<void> {
            return {};
        }

        /**
         * This function is called every time, the engine wants to redraw the window.
         *
         * @return Void or an error
         *
         * @author Cedric Hammes
         * @since  07/02/2024
         */
        [[nodiscard]] virtual auto render() noexcept -> kstd::Result<void> = 0;

        /**
         * This function returns the name of the screen
         *
         * @return Screen name
         *
         * @author Cedric Hammes
         * @since  07/02/2024
         */
        [[nodiscard]] auto get_name() const noexcept -> const std::string&;
    };
}// namespace aetherium