#pragma once

#include <compare>
#include <cstdint>

struct Row {
    uint32_t id;
    char username[32];
    char email[256];

    static auto from(const Row* src) -> Row;

    auto serialize(Row* dest) -> void;

    auto operator<=>(const Row&) const = default;
};

constexpr size_t ROW_SIZE = sizeof(Row);
