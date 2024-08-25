#pragma once

#include <cstddef>

struct Cursor {
    size_t page_num{};
    size_t cell_num{};
    bool eot{};
};
