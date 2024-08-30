#pragma once

#include <array>
#include <cstdint>
#include <span>

struct Row {
    uint32_t id;
    std::array<char, 32> username;
    std::array<char, 256> email;

    auto serialize_into(std::span<char> dest) -> void;
    auto deserialize_from(std::span<char> src) -> void;

    auto operator<=>(const Row&) const = default;
};

constexpr size_t ROW_SIZE = sizeof(Row);
