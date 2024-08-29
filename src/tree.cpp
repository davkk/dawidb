#include "tree.hpp"

#include <print>

void Node::show() {
    std::println("leaf (size: {})", header.num_cells);
    for (size_t idx{0}; idx < header.num_cells; idx++) {
        std::println(" - {} : {}", idx, cells[idx].key);
    }
}

void Node::insert(const Cursor& cursor, const Cell&& cell) {
    if (header.num_cells >= MAX_NODE_CELLS) {
        std::println(stderr, "TODO: node splitting");
        std::abort();
    }

    // make room for new cell
    if (cursor.cell_num < header.num_cells) {
        for (size_t idx{header.num_cells}; idx > cursor.cell_num; idx--) {
            // move cell over to the right
            cells[idx] = cells[idx - 1];
        }
    }

    header.num_cells++;
    cells[cursor.cell_num] = cell;
}
