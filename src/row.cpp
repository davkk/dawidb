#include "row.hpp"

#include <cassert>
#include <cstring>
#include <span>

auto Row::serialize_into(const std::span<char> dest) -> void {
    assert(dest.size() == ROW_SIZE);
    std::memcpy(dest.data(), this, ROW_SIZE);
}

auto Row::deserialize_from(const std::span<char> src) -> void {
    assert(src.size() == ROW_SIZE);
    std::memcpy(this, src.data(), ROW_SIZE);
}
