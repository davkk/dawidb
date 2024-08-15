#pragma once

#include <cstdint>
#include <span>

#include "const.hpp"

struct Row {
    uint32_t id;
    std::array<char, USERNAME_SIZE> username;
    std::array<char, EMAIL_SIZE> email;

    void serialize_into(std::span<char> dest);
    void deserialize_from(std::span<char> src);
};
