#include "row.hpp"

#include <cassert>
#include <cstring>
#include <span>

#include "const.hpp"

void Row::serialize_into(const std::span<char> dest) {
    assert(dest.size() == ROW_SIZE);
    std::memcpy(dest.data(), this, ROW_SIZE);
}

void Row::deserialize_from(const std::span<char> src) {
    assert(src.size() == ROW_SIZE);
    std::memcpy(this, src.data(), ROW_SIZE);
}
