#pragma once

#include <cstdint>
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

    void show();
    uint32_t find_cell(uint32_t key);
    void insert(const Cursor& cursor, const Cell&& cell);
};
