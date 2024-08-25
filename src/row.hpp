#pragma once

#include <array>
#include <cstdint>
#include <span>

struct Row {
    uint32_t id;
    std::array<char, 32> username;
    std::array<char, 256> email;

    void serialize_into(std::span<char> dest);
    void deserialize_from(std::span<char> src);
};
