#pragma once

#include <array>

#include "const.hpp"

using Page = std::array<char, PAGE_SIZE>;

enum class PagerError {
    OUT_OF_RANGE,
};
