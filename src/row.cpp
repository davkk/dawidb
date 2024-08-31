#include "row.hpp"

#include <cassert>
#include <cstring>

auto Row::from(const Row* const src) -> Row {
    Row row{};
    std::memcpy(&row, src, ROW_SIZE);
    return row;
}

auto Row::serialize(Row* const dest) -> void {
    std::memcpy(dest, this, ROW_SIZE);
}
