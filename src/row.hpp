#pragma once

#include <cstdint>
#include <span>

#include "const.hpp"

struct Row {
    uint32_t id;
    std::array<char, SIZE_USERNAME> username;
    std::array<char, SIZE_EMAIL> email;

    void serialize_into(std::span<char> dest);
    void deserialize_from(std::span<char> src);
};
