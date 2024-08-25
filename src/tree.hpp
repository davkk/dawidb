#pragma once

#include <memory>
#include <print>

#include "const.hpp"
#include "cursor.hpp"
#include "row.hpp"

enum class NodeType : bool {
    LEAF,
    INTERNAL,
};

struct Cell {
    uint32_t key;
    Row value;
};

struct Node;

struct Header {
    NodeType type;
    bool is_root;
    size_t num_cells;
    std::shared_ptr<Node> parent;
};

constexpr size_t MAX_NODE_CELLS = (PAGE_SIZE - sizeof(Header)) / sizeof(Cell);

struct Node {
    Header header;
    std::array<Cell, MAX_NODE_CELLS> cells;

    void insert(const Cursor& cursor, const Cell&& cell) {
        if (header.num_cells >= MAX_NODE_CELLS) {
            std::println(stderr, "TODO: node splitting");
            std::abort();
        }

        // make room for new cell
        if (cursor.cell_num < header.num_cells) {
            for (size_t idx{header.num_cells}; idx > cursor.cell_num; idx--) {
                // move cell over to the right
                // std::memcpy(&cells[idx], &cells[idx - 1], sizeof(Cell));
                cells[idx] = cells[idx - 1];
            }
        }

        header.num_cells++;
        cells[cursor.cell_num] = cell;
    }
};
