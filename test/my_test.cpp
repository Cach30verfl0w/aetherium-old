#include <gtest/gtest.h>
#include <iostream>
#include "aetherium/aetherium.hpp"

TEST(example, TestMyObject) {
    aetherium::Application application {"Test App", VK_MAKE_VERSION(1, 0, 0)};

	std::cout << "Hello unit test!\n";
}
