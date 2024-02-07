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

#include "aetherium/core/screens.hpp"

namespace aetherium::core {
    /**
     * This constructor initializes the screen with the screen name
     *
     * @param screen_name The name of the screen
     *
     * @author            Cedric Hammes
     * @since             07/02/2024
     */
    Screen::Screen(std::string screen_name) noexcept :
            _screen_name {std::move(screen_name)} {
    }

    /**
     * This function returns the name of the screen
     *
     * @return Screen name
     *
     * @author Cedric Hammes
     * @since  07/02/2024
     */
    auto Screen::get_name() const noexcept -> const std::string& {
        return _screen_name;
    }
}// namespace aetherium::core