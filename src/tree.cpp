#include "tree.hpp"

#include <print>

auto Node::show() -> void {
    std::println("leaf (size: {})", header.num_cells);
    for (auto idx{0UL}; idx < header.num_cells; idx++) {
        std::println(" - {} : {}", idx, cells[idx].key);
    }
}

auto Node::find_cell(uint32_t key) -> uint32_t {
    auto min{0UL};
    auto max{header.num_cells};

    while (min != max) {
        auto mid{(min + max) / 2};
        auto mid_value{cells[mid].key};

        if (mid_value == key) {
            return mid;
        }

        if (cells[mid].key > key) {
            max = mid;
        } else {
            min = mid + 1;
        }
    }

    return min;
}

auto Node::insert(const Cursor& cursor, const Cell& cell) -> void {
    if (header.num_cells >= MAX_NODE_CELLS) {
        std::println(stderr, "TODO: node splitting");
        std::abort();
    }

    // make room for new cell
    if (cursor.cell_num < header.num_cells) {
        for (auto idx{header.num_cells}; idx > cursor.cell_num; idx--) {
            // move cell over to the right
            cells[idx] = cells[idx - 1];
        }
    }

    header.num_cells++;
    cells[cursor.cell_num] = cell;
}
